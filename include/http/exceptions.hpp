#ifndef PROCESS_WRAPPER_INCLUDE_HTTP_EXCEPTIONS_HPP_
#define PROCESS_WRAPPER_INCLUDE_HTTP_EXCEPTIONS_HPP_

#include "protocol.hpp"
#include "tcp/excpetions.hpp"

class Exception : public tcp::Exception {
public:
  explicit Exception(const std::string &message, HttpStatusCode code);
  [[nodiscard]] HttpStatusCode getCode() const noexcept { return code_; }

private:
  HttpStatusCode code_;
};

class ParsingException : public Exception {
public:
  explicit ParsingException(const std::string &message, HttpStatusCode code);
};

class ReadingException : public Exception {
public:
  explicit ReadingException(const std::string &message);
};
class WritingException : public Exception {
public:
  explicit WritingException(const std::string &message);
};

class TimeoutException : public Exception {
public:
  explicit TimeoutException(const std::string &message);
};

#endif // PROCESS_WRAPPER_INCLUDE_HTTP_EXCEPTIONS_HPP_
