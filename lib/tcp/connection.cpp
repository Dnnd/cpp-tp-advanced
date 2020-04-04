#include "tcp/connection.hpp"
#include "tcp/excpetions.hpp"
#include "tcp/utils.hpp"
#include <cstring>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

using namespace std::string_literals;
namespace tcp {
constexpr int FD_CLOSED = -1;

Connection::Connection(int fd)
    : fd_{fd}, local_socket{get_local_sockaddr(fd)},
      remote_socket{get_remote_sockaddr(fd)} {}

void Connection::close() {
  if (fd_ == FD_CLOSED) {
    return;
  }
  if (shutdown(fd_, SHUT_RDWR) == -1) {
    throw SocketException("unable to shutdown socket: "s + std::strerror(errno),
                          local_socket, remote_socket);
  }
  if (::close(fd_) == -1) {
    throw SocketException("unable to close socket: "s + std::strerror(errno),
                          local_socket, remote_socket);
  }
  fd_ = FD_CLOSED;
}

Connection::~Connection() noexcept {
  try {
    close();
  } catch (SocketException &e) {
  }
}

Connection::Connection(const std::string &hostname, std::uint16_t port,
                       time_t timeout_sec)
    : local_socket{}, remote_socket{}, fd_{FD_CLOSED} {
  addrinfo_raii addrinfo_list = resolve_ipv4_tcp(hostname, port);
  for (const addrinfo *addr_node = addrinfo_list.get(); addr_node != nullptr;
       addr_node = addr_node->ai_next) {
    int fd = socket(addr_node->ai_family, addr_node->ai_socktype,
                    addr_node->ai_protocol);
    if (fd == -1) {
      continue;
    }
    if (::connect(fd, addr_node->ai_addr, addr_node->ai_addrlen) == 0) {
      local_socket = Sockinfo{get_local_sockaddr(fd)};
      remote_socket = Sockinfo{get_remote_sockaddr(fd)};
      setTimeout(fd, timeout_sec);
      fd_ = fd;
      return;
    }
    ::close(fd);
  }
  throw SocketOpeningException("unable to connect to endpoint", hostname, port);
}

std::size_t Connection::read(void *data, std::size_t len) {
  if (!isOpened()) {
    throw SocketException("attempt to read from closed socket", local_socket,
                          remote_socket);
  }
  ssize_t got = ::read(fd_, data, len);
  if (got == -1) {
    throw SocketException("unable to perform Connection::read "s +
                              std::strerror(errno),
                          local_socket, remote_socket);
  }
  if (got == 0) {
    close();
  }
  return got;
}
std::size_t Connection::write(const void *data, std::size_t len) {
  if (!isOpened()) {
    throw SocketException("attempt to write to closed socket", local_socket,
                          remote_socket);
  }
  ssize_t sent = ::write(fd_, data, len);
  if (sent == -1) {
    throw SocketException(
        "fail to perform write() syscall in Connection::write "s +
            std::strerror(errno),
        local_socket, remote_socket);
  }
  return sent;
}
void Connection::readExact(void *data, std::size_t len) {
  std::size_t total = 0;
  while (total < len) {
    std::size_t got = read(static_cast<char *>(data) + total, len - total);
    if (total != len && got == 0) {
      throw SocketException("EOF encountered in readExact method, read "s +
                                std::to_string(total) + " bytes",
                            local_socket, remote_socket);
    }
    total += got;
  }
}

void Connection::writeExact(const void *data, std::size_t len) {
  std::size_t total = 0;
  while (total < len) {
    std::size_t sent =
        write(static_cast<const char *>(data) + total, len - total);
    if (total != len && sent == 0) {
      throw SocketException("EOF encountered in writeExact method, wrote "s +
                                std::to_string(total) + " bytes",
                            local_socket, remote_socket);
    }
    total += sent;
  }
}
void Connection::setTimeout(time_t timeout_sec) {
  if (!isOpened()) {
    throw SocketException("unable to set timeout on closed socket",
                          local_socket, remote_socket);
  }
  setTimeout(fd_, timeout_sec);
}

Connection::Connection(Connection &&other) noexcept
    : fd_{other.fd_}, local_socket{other.local_socket},
      remote_socket{other.remote_socket} {
  other.fd_ = -1;
  other.local_socket = Sockinfo{};
  other.remote_socket = Sockinfo{};
}

Connection &Connection::operator=(Connection &&other) noexcept {
  Connection tmp{std::move(other)};
  swap(tmp);
  return *this;
}

bool Connection::isOpened() const noexcept { return fd_ != FD_CLOSED; }
void Connection::swap(Connection &other) noexcept {
  std::swap(fd_, other.fd_);
  std::swap(local_socket, other.local_socket);
  std::swap(remote_socket, other.remote_socket);
}
void Connection::setTimeout(int fd, time_t timeout_sec) const {
  timeval timeout{.tv_sec = timeout_sec};
  if (setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO | SO_RCVTIMEO, &timeout,
                 sizeof(timeout)) == -1) {
    throw SocketException("unable to set timeout on socket fd: "s +
                              std::strerror(errno),
                          local_socket, remote_socket);
  }
}
void Connection::connect(const std::string &hostname, std::uint16_t port,
                         time_t timeout_sec) {
  Connection tmp{hostname, port, timeout_sec};
  swap(tmp);
}
} // namespace tcp
