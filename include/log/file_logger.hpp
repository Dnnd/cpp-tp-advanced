#ifndef PROCESS_WRAPPER_INCLUDE_LOG_FILE_LOGGER_HPP_
#define PROCESS_WRAPPER_INCLUDE_LOG_FILE_LOGGER_HPP_
#include "log/base_logger.hpp"
#include <fstream>
namespace log {
class FileLogger : public BaseLogger {
public:
  explicit FileLogger(const std::string &log_file, Level level = Level ::ERROR);
  void flush() override;
  [[nodiscard]] std::string get_logfile() const;

protected:
  void log(const std::string &string, Level level) override;

private:
  std::ofstream sink_;
  std::string logfile_;
};
} // namespace log
#endif // PROCESS_WRAPPER_INCLUDE_LOG_FILE_LOGGER_HPP_
