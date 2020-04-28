#include "shmem/exceptions.hpp"

namespace shmem {
Exception::Exception(const std::string &message) : message_{message} {};

[[nodiscard]] const char *Exception::what() const noexcept {
  return message_.c_str();
}

ErrnoException::ErrnoException(const std::string &message, int errno_val)
    : Exception{message}, errno_val_{errno_val} {}
int ErrnoException::getErrno() const noexcept { return errno_val_; }
MutexException::MutexException(const std::string &message, int errno_val)
    : ErrnoException{message, errno_val} {}
} // namespace shmem
