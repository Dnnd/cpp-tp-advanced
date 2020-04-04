#include "log/exceptions.hpp"
namespace log {
const char *LogError::what() const noexcept { return message_.c_str(); }

LogError::LogError(std::string message) : message_{std::move(message)} {}

const std::string &FileLogError::log_file() const noexcept { return path_; }

FileLogError::FileLogError(std::string message, std::string log_file)
    : LogError{std::move(message)}, path_{std::move(log_file)} {}
} // namespace log