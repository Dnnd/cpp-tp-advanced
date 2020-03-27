#ifndef PROCESS_WRAPPER_INCLUDE_LOG_LOGGER_HPP_
#define PROCESS_WRAPPER_INCLUDE_LOG_LOGGER_HPP_
#include "base_logger.hpp"
#include "file_logger.hpp"
#include "stderr_logger.hpp"
#include "stdout_logger.hpp"
#include <memory>
namespace log{
class Logger {
public:
  static Logger &get_instance();
  Logger(const Logger &other) = delete;
  Logger(Logger &&other) = delete;
  Logger &operator=(const Logger &other) = delete;
  Logger &operator=(Logger &&other) = delete;
  void set_global_logger(std::unique_ptr<BaseLogger> global_logger);
  BaseLogger &get_global_logger();

private:
  Logger();
  std::unique_ptr<BaseLogger> global_logger_;
};

void debug(const std::string& message);
void error(const std::string& message);
void info(const std::string& message);
void warn(const std::string &message);

void set_global_logger(std::unique_ptr<BaseLogger> logger);

std::unique_ptr<BaseLogger> create_file_logger(const std::string &path);
std::unique_ptr<BaseLogger> create_stdout_logger();
std::unique_ptr<BaseLogger> create_stderr_logger();
}
#endif // PROCESS_WRAPPER_INCLUDE_LOG_LOGGER_HPP_
