#include "tcp/async_connection.hpp"
#include "tcp/excpetions.hpp"
#include "tcp/utils.hpp"
#include <cstring>
#include <string>
#include <unistd.h>
using namespace std::string_literals;

namespace tcp {
constexpr int FD_CLOSED = -1;

AsyncConnection::AsyncConnection(int fd)
    : fd_{fd}, local_socket_{get_local_sockaddr(fd)},
      remote_socket_{get_remote_sockaddr(fd)}, interest_set_{0} {}

NonBlockResult AsyncConnection::write(const char *data, size_t size) {
  if (!isOpened()) {
    throw SocketException("attempt to write to closed socket", local_socket_,
                          remote_socket_);
  }
  ssize_t sent = ::write(fd_, data, size);
  if (sent == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
    return NonBlockResult{0};
  } else if (sent == -1) {
    throw SocketException(
        "fail to perform write() syscall in AsyncConnection::write "s +
            std::strerror(errno),
        local_socket_, remote_socket_);
  }
  return NonBlockResult{static_cast<size_t>(sent)};
}

NonBlockResult AsyncConnection::read(char *data, std::size_t size) {
  if (!isOpened()) {
    throw SocketException("attempt to read from closed socket", local_socket_,
                          remote_socket_);
  }
  ssize_t got = ::read(fd_, data, size);
  if (got == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
    return NonBlockResult{0};
  } else if (got == -1) {
    throw SocketException("unable to perform AsyncConnection::read "s +
                              std::strerror(errno),
                          local_socket_, remote_socket_);
  } else if (got == 0) {
    close();
    return NonBlockResult{};
  }
  return NonBlockResult{static_cast<size_t>(got)};
}

void AsyncConnection::close() {
  if (shutdown(fd_, SHUT_RDWR) == -1) {
    throw SocketException("unable to shutdown socket: "s + std::strerror(errno),
                          local_socket_, remote_socket_);
  }
  if (::close(fd_) == -1) {
    throw SocketException("unable to close socket: "s + std::strerror(errno),
                          local_socket_, remote_socket_);
  }
  fd_ = FD_CLOSED;
}
bool AsyncConnection::isOpened() const noexcept { return fd_ != FD_CLOSED; }
void AsyncConnection::swap(AsyncConnection &other) noexcept {
  std::swap(fd_, other.fd_);
  std::swap(local_socket_, other.local_socket_);
  std::swap(remote_socket_, other.remote_socket_);
  std::swap(interest_set_, other.interest_set_);
}
AsyncConnection::AsyncConnection(AsyncConnection &&other) noexcept
    : fd_{other.fd_}, local_socket_{other.local_socket_},
      remote_socket_{other.remote_socket_}, interest_set_{other.interest_set_} {
  other.fd_ = FD_CLOSED;
  other.local_socket_ = Sockinfo{};
  other.remote_socket_ = Sockinfo{};
  other.interest_set_ = {};
}
AsyncConnection &AsyncConnection::operator=(AsyncConnection &&other) noexcept {
  AsyncConnection tmp{std::move(other)};
  swap(tmp);
  return *this;
}
AsyncConnection::~AsyncConnection() noexcept {
  try {
    close();
  } catch (SocketException &e) {
  }
}
bool AsyncConnection::triggersOn(EventsSet events) const noexcept {
  return interest_set_.haveIntersection(events);
}
void AsyncConnection::setInterestsSet(EventsSet new_set) noexcept {
  interest_set_ = new_set;
}

const EventsSet & AsyncConnection::getInterestSet() const noexcept {
  return interest_set_;
}
EventsSet &AsyncConnection::getInterestSet() noexcept {
  return interest_set_;
}

bool NonBlockResult::isEOF() const noexcept { return !result_.has_value(); }

size_t NonBlockResult::bytes() const { return result_.value(); }

bool NonBlockResult::again() const { return result_.value() == 0; }

NonBlockResult::NonBlockResult(std::size_t res) : result_{res} {}

NonBlockResult::NonBlockResult() : result_{std::nullopt} {}

} // namespace tcp