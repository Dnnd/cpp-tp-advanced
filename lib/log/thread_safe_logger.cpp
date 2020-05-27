#include "log/thread_safe_logger.hpp"

void log::ThreadSafeStdErrLogger::log(const std::string &msg, log::Level level) {
  std::lock_guard guard{lock_};
  StderrLogger::log(msg, level);
}
void log::ThreadStdoutLogger::log(const std::string &msg, log::Level level) {
  std::lock_guard guard{lock_};
  StdoutLogger::log(msg, level);
}
void log::ThreadSafeFileLogger::log(const std::string &msg, log::Level level) {
  std::lock_guard guard{lock_};
  FileLogger::log(msg, level);
}
log::ThreadSafeFileLogger::ThreadSafeFileLogger(const std::string &log_file,
                                                log::Level level)
    : FileLogger{log_file, level} {}
