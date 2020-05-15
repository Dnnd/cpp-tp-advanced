#ifndef PROCESS_WRAPPER_LIB_LOG_BASE_LOGGER_HPP_
#define PROCESS_WRAPPER_LIB_LOG_BASE_LOGGER_HPP_
#include "log/level.hpp"

namespace log {
class BaseLogger {
public:
  explicit BaseLogger(Level level_ = Level::ERROR) noexcept;
  void debug(const std::string &message);
  void error(const std::string &message);
  void info(const std::string &message);
  void warn(const std::string &message);
  void set_level(Level level_) noexcept;
  [[nodiscard]] Level get_level() const noexcept;
  virtual ~BaseLogger() = default;
  virtual void flush() = 0;
  [[nodiscard]] bool willBeLogged(Level level) const noexcept;

protected:
  virtual void log(const std::string &, Level) = 0;
private:
  Level level_;
};
} // namespace log
#endif // PROCESS_WRAPPER_LIB_LOG_BASE_LOGGER_HPP_
