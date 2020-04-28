#ifndef PROCESS_WRAPPER_INCLUDE_SHMEM_EXCEPTIONS_HPP_
#define PROCESS_WRAPPER_INCLUDE_SHMEM_EXCEPTIONS_HPP_

#include <exception>
#include <string>

namespace shmem {
class Exception : public std::exception {
public:
  explicit Exception(const std::string &message);
  [[nodiscard]] const char *what() const noexcept override;

private:
  std::string message_;
};

class ErrnoException : public Exception {
public:
  ErrnoException(const std::string &message, int errno_val);

  [[nodiscard]] int getErrno() const noexcept;

private:
  int errno_val_;
};

class MutexException : public ErrnoException {
public:
  MutexException(const std::string &message, int errno_val);
  ;
};
} // namespace shmem
#endif // PROCESS_WRAPPER_INCLUDE_SHMEM_EXCEPTIONS_HPP_
