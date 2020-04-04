#ifndef PROCESS_WRAPPER_INCLUDE_LOG_LEVEL_HPP_
#define PROCESS_WRAPPER_INCLUDE_LOG_LEVEL_HPP_

#include <ostream>
#include <string>
namespace log {
enum class Level { DEBUG = 3, INFO = 2, WARN = 1, ERROR = 0 };

std::string to_string(Level level) noexcept;

std::ostream &operator<<(std::ostream &os, Level level);
} // namespace log
#endif // PROCESS_WRAPPER_INCLUDE_LOG_LEVEL_HPP_
