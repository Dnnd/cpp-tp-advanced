#ifndef PROCESS_WRAPPER_INCLUDE_LOG_STDOUT_LOGGER_HPP_
#define PROCESS_WRAPPER_INCLUDE_LOG_STDOUT_LOGGER_HPP_
#include "log/base_logger.hpp"
namespace log {
class StdoutLogger : public BaseLogger {
public:
  void flush() override;

protected:
  void log(const std::string &string, Level level) override;
};
} // namespace log
#endif // PROCESS_WRAPPER_INCLUDE_LOG_STDOUT_LOGGER_HPP_
