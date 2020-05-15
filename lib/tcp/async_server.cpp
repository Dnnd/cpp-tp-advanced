#include "tcp/async_server.hpp"
#include "tcp/excpetions.hpp"
#include "tcp/utils.hpp"
#include <cstring>
#include <iostream>
#include <string>
#include <sys/epoll.h>
#include <utility>
#include <zconf.h>
using namespace std::string_literals;
namespace tcp {
constexpr int FD_CLOSED = -1;
constexpr int POLLER_EVENTS_SIZE = 256;

void AsyncServer::open(const std::string &hostname, uint16_t port,
                       EventsSet default_interests_set) {
  AsyncServer server{hostname, port, handler_, default_interests_set};
  swap(server);
}

AsyncServer::AsyncServer(const std::string &hostname, uint16_t port,
                         Callback handler, EventsSet interests_set,
                         int timeout_usec)
    : fd_{FD_CLOSED}, socket_{}, poller_{POLLER_EVENTS_SIZE},
      handler_{std::move(handler)},
      default_interests_set_{interests_set}, timeout_{timeout_usec} {
  addrinfo_ptr addrinfo_list = resolve_ipv4_tcp(hostname, port);
  for (const addrinfo *addr_node = addrinfo_list.get(); addr_node != nullptr;
       addr_node = addr_node->ai_next) {
    int fd =
        socket(addr_node->ai_family, addr_node->ai_socktype | SOCK_NONBLOCK,
               addr_node->ai_protocol);
    if (fd == -1) {
      continue;
    }
    if (bind(fd, addr_node->ai_addr, addr_node->ai_addrlen) == 0 &&
        listen(fd, SOMAXCONN) == 0) {
      socket_ = Sockinfo{*reinterpret_cast<sockaddr_in *>(addr_node->ai_addr)};
      fd_ = fd;
      return;
    }
    ::close(fd);
  }
  throw SocketOpeningException("unable to bind", hostname, port);
}
void AsyncServer::setMaxConnect(int connmax) {
  if (listen(fd_, connmax) == -1) {
    throw ServerSocketException("unable to change max connections to "s +
                                    std::strerror(connmax) + ' ' +
                                    std::strerror(errno),
                                socket_);
  }
}

AsyncServer::AsyncServer(AsyncServer &&other) noexcept
    : fd_{other.fd_}, socket_{other.socket_}, poller_{std::move(other.poller_)},
      handler_{std::move(other.handler_)},
      default_interests_set_{other.default_interests_set_},
      timeout_{other.timeout_} {
  other.fd_ = FD_CLOSED;
  other.socket_ = Sockinfo{};
  other.default_interests_set_ = {};
}

AsyncServer &AsyncServer::operator=(AsyncServer &&other) noexcept {
  AsyncServer tmp{std::move(other)};
  swap(tmp);
  return *this;
}

void AsyncServer::close() {
  if (!isOpened()) {
    return;
  }
  if (-1 == ::close(fd_)) {
    throw ServerSocketException(
        "unable to close server: "s + std::strerror(errno), socket_);
  };
  fd_ = FD_CLOSED;
}

AsyncServer::~AsyncServer() noexcept {
  try {
    close();
  } catch (ServerSocketException &e) {
  }
}
void AsyncServer::swap(AsyncServer &other) noexcept {
  std::swap(fd_, other.fd_);
  std::swap(socket_, other.socket_);
  std::swap(timeout_, other.timeout_);
  poller_.swap(other.poller_);
}
bool AsyncServer::isOpened() const noexcept { return fd_ != FD_CLOSED; }
Sockinfo AsyncServer::getServerSocketInfo() const noexcept { return socket_; }

void AsyncServer::accept() {
  poller_.add(fd_, {Events::IN});
  while (isOpened()) {
    Span<epoll_event> events = poller_.wait();
    for (epoll_event *event = events.data; event != events.data + events.size;
         ++event) {
      if (event->data.fd == fd_) {
        acceptConnections();
      } else {
        try {
          handleEvent(*event);
        } catch (...) {
          closeConnection(event->data.fd);
        }
      }
    };
  }
  poller_.remove(fd_);
}
void AsyncServer::acceptConnections() {
  while (true) {
    sockaddr_in client_sock{};
    socklen_t sock_size = sizeof(client_sock);
    int clientfd = ::accept4(fd_, reinterpret_cast<sockaddr *>(&client_sock),
                             &sock_size, SOCK_NONBLOCK);
    if (clientfd == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
      return;
    }
    // эти ошибки считаются не критичными
    if (clientfd == -1 &&
        (errno == ECONNABORTED || errno == EINTR || errno == EPERM)) {
      continue;
    }
    // остальные ошибки считаются критичными (слишком сложно обрабатывать в
    // рамках этого задания) цикл приема и обработки соединений прерывается
    if (clientfd == -1) {
      throw ServerSocketException(
          "unable to accept connection: "s + std::strerror(errno), socket_);
    }
    registerConnection(clientfd);
  }
}

void AsyncServer::handleEvent(const epoll_event &event) {
  std::shared_ptr<AsyncConnection> connection = getConnection(event.data.fd);

  EventsSet interests_before = connection->getInterestSet();
  EventsSet events_set{event.events};
  if (connection->triggersOn(events_set) ||
      events_set.haveIntersection({Events::HUP, Events::RDHUP, Events ::ERR})) {
    handler_(connection, events_set);
  }
  if (!connection->isOpened()) {
    connections_.erase(event.data.fd);
  } else {
    EventsSet interests_after = connection->getInterestSet();
    if (!interests_after.diff(interests_before).empty()) {
      poller_.modify(event.data.fd, interests_after);
    }
  }
}
std::shared_ptr<AsyncConnection> AsyncServer::getConnection(int fd) const {
  auto con_iterator = connections_.find(fd);
  if (con_iterator == connections_.end()) {
    throw Exception("unexpected state: event on non-existent connection");
  }
  auto &&[_, connection] = *con_iterator;
  return connection;
}
void AsyncServer::closeConnection(int fd) {
  auto node = connections_.extract(fd);
  if (!node.empty()) {
    node.mapped()->close();
  }
}

void AsyncServer::registerConnection(int clientfd) {
  // make_shared нельзя, т.к. AsyncConnection(int fd) - приватный конструктор
  auto connection =
      std::shared_ptr<AsyncConnection>(new AsyncConnection{clientfd});
  connection->setInterestsSet(default_interests_set_);
  poller_.add(clientfd, connection->getInterestSet());
  connections_.insert({clientfd, std::move(connection)});
}

} // namespace tcp