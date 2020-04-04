#include "log/stderr_logger.hpp"
#include "log/exceptions.hpp"
#include "log/message.hpp"
#include <iostream>
namespace log {
void StderrLogger::flush() {
  // no-op, потому что std::cerr не буферизуется
}

void StderrLogger::log(const std::string &message, Level level) {
  if (willBeLogged(level)) {
    std::cerr << LogMessage{.level = level, .text = message} << '\n';
    if (!std::cerr) {
      throw LogError("unable to write message to stderr");
    }
  }
}
} // namespace log