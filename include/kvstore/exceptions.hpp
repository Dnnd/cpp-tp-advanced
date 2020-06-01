#ifndef PROCESS_WRAPPER_INCLUDE_KVSTORE_EXCEPTIONS_HPP_
#define PROCESS_WRAPPER_INCLUDE_KVSTORE_EXCEPTIONS_HPP_
#include <exception>
#include <string>

class Exception : public std::exception {
public:
  explicit Exception(const std::string &msg);
  [[nodiscard]] const char *what() const noexcept override;

private:
  std::string message_;
};

class ErrnoException : public Exception {
public:
  ErrnoException(const std::string &msg, int errno_code);

  [[nodiscard]] int get_errno() const noexcept;

private:
  std::string message_;
  int errno_code_;
};

#endif // PROCESS_WRAPPER_INCLUDE_KVSTORE_EXCEPTIONS_HPP_
