#include "tcp/server.hpp"
#include "tcp/excpetions.hpp"
#include "tcp/utils.hpp"
#include <cstring>
#include <string>
using namespace std::string_literals;
namespace tcp {
constexpr int FD_CLOSED = -1;

void Server::open(const std::string &hostname, uint16_t port) {
  Server server{hostname, port};
  std::swap(*this, server);
}
Server::Server(const std::string &hostname, uint16_t port)
    : fd_{-1}, socket_{} {
  addrinfo_raii addrinfo_list = resolve_ipv4_tcp(hostname, port);
  for (const addrinfo *addr_node = addrinfo_list.get(); addr_node != nullptr;
       addr_node = addr_node->ai_next) {
    int fd = socket(addr_node->ai_family, addr_node->ai_socktype,
                    addr_node->ai_protocol);
    if (fd == -1) {
      continue;
    }
    if (bind(fd, addr_node->ai_addr, addr_node->ai_addrlen) == 0 &&
        listen(fd, SOMAXCONN) == 0) {
      socket_ = *reinterpret_cast<sockaddr_in *>(addr_node->ai_addr);
      fd_ = fd;
      return;
    }
    ::close(fd);
  }
  throw SocketOpeningException("unable to bind", hostname, port);
}
void Server::setMaxConnect(int connmax) {
  if (listen(fd_, connmax) == -1) {
    throw ServerSocketException("unable to change max connections to "s +
                                    std::strerror(connmax) + ' ' +
                                    std::strerror(errno),
                                socket_);
  }
}
Connection Server::accept() const {
  if (!isOpened()) {
    throw ServerSocketException("attempt to listen on closed socket"s, socket_);
  }
  sockaddr_in client_sock{};
  socklen_t sock_size = sizeof(client_sock);
  int clientfd =
      ::accept(fd_, reinterpret_cast<sockaddr *>(&client_sock), &sock_size);
  if (clientfd == -1) {
    throw ServerSocketException(
        "unable to accept connection: "s + std::strerror(errno), socket_);
  }
  return Connection{clientfd};
}

Server::Server(Server &&other) noexcept
    : fd_{other.fd_}, socket_{other.socket_} {
  other.fd_ = FD_CLOSED;
  other.socket_ = sockaddr_in{};
}

Server &Server::operator=(Server &&other) noexcept {
  Server tmp{std::move(other)};
  swap(tmp);
  return *this;
}

void Server::close() {
  if (!isOpened()) {
    return;
  }
  if (-1 == ::close(fd_)) {
    throw ServerSocketException(
        "unable to close server: "s + std::strerror(errno), socket_);
  };
  fd_ = FD_CLOSED;
}

Server::~Server() noexcept {
  try {
    close();
  } catch (std::runtime_error &e) {
  }
}
void Server::swap(Server &other) noexcept {
  std::swap(fd_, other.fd_);
  std::swap(socket_, other.socket_);
}
bool Server::isOpened() const noexcept { return fd_ != FD_CLOSED; };
} // namespace tcp