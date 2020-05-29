#include "kvstore/exceptions.hpp"

Exception::Exception(const std::string &msg) : message_{msg} {}
const char *Exception::what() const noexcept { return message_.c_str(); }
ErrnoException::ErrnoException(const std::string &msg, int errno_code)
    : Exception(msg), errno_code_{errno_code} {}
int ErrnoException::get_errno() const noexcept { return errno_code_; }
