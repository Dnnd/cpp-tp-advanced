#include "log/file_logger.hpp"
#include "log/exceptions.hpp"
#include "log/message.hpp"
namespace log {
void FileLogger::flush() { sink_.flush(); }

void FileLogger::log(const std::string &message, Level level) {
  if (willBeLogged(level)) {
    sink_ << LogMessage{.level = level, .text = message} << '\n';
    if (!sink_) {
      throw FileLogError("unable to write message to file", logfile_);
    }
  }
}

FileLogger::FileLogger(const std::string &log_file, Level level)
    : BaseLogger{level}, sink_{log_file}, logfile_{log_file} {
  if (!sink_.is_open()) {
    throw FileLogError("unable to open file for writing", log_file);
  }
}
const std::string &FileLogger::get_logfile() const noexcept { return logfile_; }
} // namespace log