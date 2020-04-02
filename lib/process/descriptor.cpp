#include "process/descriptor.hpp"
#include <cstddef>
#include <cstring>
#include <fcntl.h>
#include <stdexcept>
#include <unistd.h>

using namespace std::string_literals;

static constexpr int FD_CLOSED = -1;

Descriptor::~Descriptor() noexcept {
  try {
    close();
  } catch (std::runtime_error &e) {
    // намеренно игнорируем исключения в деструкторе
  }
}

void Descriptor::close() {
  if (fd_ == FD_CLOSED) {
    return;
  }
  int ret = ::close(fd_);
  if (ret == -1) {
    throw std::runtime_error(
        "unable to close descriptor in Descriptor::close: "s +
        std::to_string(fd_) + " " + std::strerror(errno));
  }
  fd_ = FD_CLOSED;
}

Descriptor::Descriptor(int fd) : fd_{fd} {}

Descriptor::Descriptor(Descriptor &&other) noexcept : fd_{other.fd_} {
  other.fd_ = FD_CLOSED;
}

void Descriptor::bind(int new_fd) {
  if (fd_ == FD_CLOSED) {
    throw std::runtime_error("unable to bind file descriptor to closed file "
                             "descriptor in Descriptor::bind");
  }
  if (dup2(fd_, new_fd) == -1) {
    throw std::runtime_error(
        "fail to perform dup2() syscall in Descriptor::bind on :"s +
        std::to_string(new_fd) + ": " + std::strerror(errno));
  }
}

Descriptor &Descriptor::operator=(Descriptor &&other) noexcept {
  close();
  fd_ = other.fd_;
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
int Descriptor::getUnderlyingDescriptor() const noexcept { return fd_; }
