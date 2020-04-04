#ifndef PROCESS_WRAPPER_INCLUDE_LOG_MESSAGE_HPP_
#define PROCESS_WRAPPER_INCLUDE_LOG_MESSAGE_HPP_
#include "level.hpp"
#include <ostream>
namespace log {
struct LogMessage {
  Level level;
  std::string_view text;
};

std::ostream &operator<<(std::ostream &os, const LogMessage &message);
} // namespace log
#endif // PROCESS_WRAPPER_INCLUDE_LOG_MESSAGE_HPP_
