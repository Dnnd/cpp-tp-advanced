#include "http/exceptions.hpp"

Exception::Exception(const std::string &message, HttpStatusCode code)
    : tcp::Exception{message}, code_{code} {}

ParsingException::ParsingException(const std::string &message,
                                   HttpStatusCode code)
    : Exception{message, code} {}

ReadingException::ReadingException(const std::string &message)
    : Exception{message, HttpStatusCode::INTERNAL_SERVER_ERROR} {}

WritingException::WritingException(const std::string &message)
    : Exception{message, HttpStatusCode::INTERNAL_SERVER_ERROR} {}

TimeoutException::TimeoutException(const std::string &message)
    : Exception{message, HttpStatusCode::REQUEST_TIMEOUT} {}
