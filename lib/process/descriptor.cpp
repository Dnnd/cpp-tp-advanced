#include <cstddef>
#include <cstring>
#include <fcntl.h>
#include <process/descriptor.hpp>
#include <stdexcept>
#include <unistd.h>

using namespace std::string_literals;

Descriptor::~Descriptor() { close(); }

void Descriptor::close() noexcept {
  if (fd_ != FD_CLOSED) {
    ::close(fd_);
    fd_ = FD_CLOSED;
  }
}

Descriptor::Descriptor(int fd) : fd_{fd} {}

Descriptor::Descriptor(Descriptor &&other) noexcept : fd_{other.fd_} {
  other.fd_ = FD_CLOSED;
}

void Descriptor::bind(int new_fd) {
  if (fd_ == FD_CLOSED) {
    this->fd_ = new_fd;
    return;
  }
  if (dup2(fd_, new_fd) == -1) {
    throw std::runtime_error(
        "fail to perform dup2() syscall in Descriptor::bind on :"s +
        std::to_string(new_fd) + ": " + std::strerror(errno));
  }
  close();
  fd_ = new_fd;
}

Descriptor &Descriptor::operator=(Descriptor &&other) noexcept {
  close();
  this->fd_ = other.fd_;
  other.fd_ = FD_CLOSED;
  return *this;
}

Descriptor::Descriptor() : fd_{FD_CLOSED} {}

bool Descriptor::isClosed() const {
  if (fd_ == FD_CLOSED) {
    return true;
  }
  int status = fcntl(fd_, F_GETFD);
  if (status == -1) {
    if (errno == EBADF) {
      return true;
    }
    throw std::runtime_error(
        "failed fcntl() syscall in Descriptor::isClosed: "s +
        std::strerror(errno));
  }
  return false;
}
Descriptor::operator int() const { return fd_; }
