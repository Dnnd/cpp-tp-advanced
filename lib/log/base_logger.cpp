#include "log/base_logger.hpp"
namespace log {
void BaseLogger::set_level(Level level) noexcept { level_ = level; }

BaseLogger::BaseLogger(Level level) noexcept : level_{level} {}

void BaseLogger::debug(const std::string &message) {
  log(message, Level::DEBUG);
}
void BaseLogger::error(const std::string &message) {
  log(message, Level::ERROR);
}

void BaseLogger::info(const std::string &message) { log(message, Level::INFO); }

void BaseLogger::warn(const std::string &message) { log(message, Level::WARN); }

Level BaseLogger::get_level() const noexcept { return level_; }

[[nodiscard]] bool BaseLogger::willBeLogged(Level level) const noexcept {
  return level <= level_;
}
} // namespace log