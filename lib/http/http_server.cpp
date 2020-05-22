#include "http/http_server.hpp"
#include "http/http_worker.hpp"
#include "http/utils.hpp"
#include "tcp/utils.hpp"
#include <forward_list>
#include <iostream>
#include <netinet/tcp.h>
#include <string>
#include <unistd.h>
using namespace std::string_literals;

HttpServer::HttpServer(const std::string &hostname, uint16_t port,
                       size_t thread_pool_size,
                       std::chrono::milliseconds timeout,
                       std::unique_ptr<log::BaseLogger> logger)
    : thread_pool_size_{thread_pool_size}, timeout_{timeout},
      logger_{std::move(logger)}, closed_{false} {
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
void HttpServer::run(bool enable_graceful_shutdown) {

  // не vector, т.к. HttpWorker - не copyable и не movable
  std::forward_list<HttpWorker> thread_pool_;
  for (int i = 0; i < thread_pool_size_; ++i) {
    thread_pool_.emplace_front(
        closed_, fd_, timeout_,
        [this](HttpRequest &request) { return serveRequest(request); },
        *logger_);
  }
  if (enable_graceful_shutdown) {
    sigset_t signals_;
    sigaddset(&signals_, SIGTERM);
    sigaddset(&signals_, SIGINT);
    int got = 0;
    sigwait(&signals_, &got);
    closed_.store(true);
  }
}

HttpServer::~HttpServer() noexcept { close(fd_); }