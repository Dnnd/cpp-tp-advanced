#include "log/level.hpp"
namespace log {
std::string to_string(Level level) noexcept {
  switch (level) {
  case Level::DEBUG:
    return "DEBUG";
  case Level::INFO:
    return "INFO";
  case Level::WARN:
    return "WARN";
  case Level::ERROR:
    return "ERROR";
  }
  return "";
}
std::ostream &operator<<(std::ostream &os, Level level) {
  return os << to_string(level);
}
} // namespace log