#ifndef PROCESS_WRAPPER_INCLUDE_LOG_THREAD_SAFE_LOGGER_HPP_
#define PROCESS_WRAPPER_INCLUDE_LOG_THREAD_SAFE_LOGGER_HPP_

#include "log/file_logger.hpp"
#include "stderr_logger.hpp"
#include "stdout_logger.hpp"
#include <memory>
#include <mutex>

namespace log {

class ThreadSafeFileLogger : public FileLogger {
public:
  explicit ThreadSafeFileLogger(const std::string &log_file,
                                Level level = Level ::ERROR);
  ~ThreadSafeFileLogger() override = default;

protected:
  void log(const std::string &msg, Level level) override;

private:
  mutable std::mutex lock_;
};

class ThreadSafeStdErrLogger : public log::StderrLogger {
public:
  using StderrLogger::StderrLogger;
  ~ThreadSafeStdErrLogger() override = default;

protected:
  void log(const std::string &msg, Level level) override;

private:
  mutable std::mutex lock_;
};

class ThreadStdoutLogger : public log::StdoutLogger {
public:
  ~ThreadStdoutLogger() override = default;

protected:
  void log(const std::string &msg, Level level) override;

private:
  mutable std::mutex lock_;
};
} // namespace log
#endif // PROCESS_WRAPPER_INCLUDE_LOG_THREAD_SAFE_LOGGER_HPP_
