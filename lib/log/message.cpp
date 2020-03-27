#include "log/message.hpp"
namespace log {
std::ostream &operator<<(std::ostream &os, const LogMessage &message) {
  return os << '[' << message.level << "]: " << message.text;
}
} // namespace log