#ifndef PROCESS_WRAPPER_INCLUDE_LOG_EXCEPTIONS_HPP_
#define PROCESS_WRAPPER_INCLUDE_LOG_EXCEPTIONS_HPP_
#include <exception>
#include <string>

namespace log {
class LogError : public std::exception {
public:
  explicit LogError(std::string message);
  [[nodiscard]] const char *what() const noexcept override;

private:
  std::string message_;
};

class FileLogError : public LogError {
public:
  FileLogError(std::string message, std::string log_file);
  [[nodiscard]] std::string log_file() const noexcept;

private:
  std::string path_;
};
} // namespace log
#endif // PROCESS_WRAPPER_INCLUDE_LOG_EXCEPTIONS_HPP_
