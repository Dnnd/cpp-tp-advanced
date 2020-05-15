#include "http/http_server.hpp"
#include "http/http_worker.hpp"
#include "http/utils.hpp"
#include "tcp/sockinfo.hpp"
#include "tcp/utils.hpp"
#include <string>
#include <unistd.h>
using namespace std::string_literals;
namespace {
constexpr size_t EVENTS_PER_POLLER = 1024;

}

std::atomic_bool HttpServer::closed_{false};

HttpServer::HttpServer(const std::string &hostname, uint16_t port,
                       size_t thread_pool_size,
                       std::chrono::milliseconds timeout,
                       std::unique_ptr<log::BaseLogger> logger)
    : accept_poller_(1), thread_pool_size_{thread_pool_size}, timeout_{timeout},
      logger_{std::move(logger)} {
  tcp::addrinfo_ptr addrinfo_list = tcp::resolve_ipv4_tcp(hostname, port);
  for (const addrinfo *addr_node = addrinfo_list.get(); addr_node != nullptr;
       addr_node = addr_node->ai_next) {
    int fd =
        socket(addr_node->ai_family, addr_node->ai_socktype | SOCK_NONBLOCK,
               addr_node->ai_protocol);
    if (fd == -1) {
      continue;
    }
    int enable = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
    if (bind(fd, addr_node->ai_addr, addr_node->ai_addrlen) == 0 &&
        listen(fd, SOMAXCONN) == 0) {
      fd_ = fd;
      return;
    }
    ::close(fd);
  }
  throw std::runtime_error("unable to bind to addr\n");
}
void HttpServer::run() {
  std::vector<JThread> thread_pool_;
  thread_pool_.reserve(thread_pool_size_);
  accept_poller_.add(fd_, {Events::IN, Events::EXCLUSIVE});

  for (int i = 0; i < thread_pool_size_; ++i) {
    pollers_pool_.emplace_back(EVENTS_PER_POLLER);
  }

  for (int i = 0; i < thread_pool_size_; ++i) {
    thread_pool_.emplace_back(std::thread([&poller = pollers_pool_[i], this] {
      HttpWorker{poller, std::chrono::milliseconds(timeout_),
                 [this](HttpRequest &request) { return serveRequest(request); },
                 *logger_}();
    }));
  }

  acceptClients();

  thread_pool_.clear();
  accept_poller_.remove(fd_);
}
void HttpServer::acceptClients() {
  unsigned balance_idx = 0;
  while (!closed_) {
    try {
      accept_poller_.wait();
      while (true) {
        sockaddr_in client_sock{};
        socklen_t sock_size = sizeof(client_sock);
        int clientfd =
            ::accept4(fd_, reinterpret_cast<sockaddr *>(&client_sock),
                      &sock_size, SOCK_NONBLOCK);
        if (clientfd == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
          break;
        }
        if (clientfd == -1 &&
            (errno == ECONNABORTED || errno == EINTR || errno == EPERM)) {
          continue;
        }
        if (clientfd == -1) {
          logger_->error("unable to accept connection");
          continue;
        }

        pollers_pool_[balance_idx++].add(
            clientfd, {Events::IN, Events::EDGE, Events::ONESHOT});

        balance_idx %= pollers_pool_.size();
      }
    } catch (tcp::InterruptedException &ex) {
      continue;
    }
  }
}
void HttpServer::stop() { closed_.store(true); }
HttpServer::~HttpServer() noexcept { close(fd_); }

void HttpServer::enableGracefulShutdown() {
  struct sigaction sigIntHandler {};
  sigIntHandler.sa_sigaction = &HttpServer::_stop;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;
  sigaction(SIGTERM, &sigIntHandler, NULL);
  struct sigaction ignore_sigpipe {
    SIG_IGN
  };
  sigaction(SIGPIPE, &ignore_sigpipe, NULL);
}
