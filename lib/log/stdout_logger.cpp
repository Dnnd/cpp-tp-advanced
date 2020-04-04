#include "log/stdout_logger.hpp"
#include "log/exceptions.hpp"
#include "log/message.hpp"
#include <iostream>
namespace log {
void StdoutLogger::flush() { std::cout.flush(); }
void StdoutLogger::log(const std::string &message, Level level) {
  if (willBeLogged(level)) {
    std::cout << LogMessage{.level = level, .text = message} << '\n';
    if (!std::cout) {
      throw LogError("unable to write message to stdout");
    }
  }
}
} // namespace log