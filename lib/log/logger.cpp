#include "log/logger.hpp"
#include <stdexcept>
namespace log {
Logger &Logger::get_instance() {
  static Logger logger_;
  return logger_;
}
Logger::Logger() : global_logger_{create_stderr_logger()} {}

void Logger::set_global_logger(std::unique_ptr<BaseLogger> global_logger) {
  if (!global_logger) {
    throw std::runtime_error("set_global_logger called with nullptr");
  }
  global_logger_ = std::move(global_logger);
}

BaseLogger &Logger::get_global_logger() { return *global_logger_; }

void debug(const std::string &message) {
  Logger::get_instance().get_global_logger().debug(message);
}

void error(const std::string &message) {
  Logger::get_instance().get_global_logger().error(message);
}

void info(const std::string &message) {
  Logger::get_instance().get_global_logger().info(message);
}

void warn(const std::string &message) {
  Logger::get_instance().get_global_logger().warn(message);
}

std::unique_ptr<BaseLogger> create_file_logger(const std::string &path) {
  return std::make_unique<FileLogger>(path);
}

std::unique_ptr<BaseLogger> create_stdout_logger() {
  return std::make_unique<StdoutLogger>();
}

std::unique_ptr<BaseLogger> create_stderr_logger() {
  return std::make_unique<StderrLogger>();
}

void set_global_logger(std::unique_ptr<BaseLogger> logger) {
  Logger::get_instance().set_global_logger(std::move(logger));
}
} // namespace log