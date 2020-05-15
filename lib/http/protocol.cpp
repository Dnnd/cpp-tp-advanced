#include "http/protocol.hpp"

std::ostream &operator<<(std::ostream &os, const HttpVersion &version) {
  switch (version) {
  case HttpVersion::HTTP1_0:
    os << "HTTP/1.0";
    return os;
  case HttpVersion::HTTP1_1:
    os << "HTTP/1.1";
    return os;
  default:
    return os;
  }
}
std::ostream &operator<<(std::ostream &os, const HttpMethod &version) {
  switch (version) {
  case HttpMethod::GET:
    os << "GET";
    return os;
  case HttpMethod::POST:
    os << "POST";
    return os;
  case HttpMethod::PUT:
    os << "PUT";
    return os;
  case HttpMethod::DELETE:
    os << "DELETE";
    return os;
  default:
    return os;
  }
}
bool CaseInsensitiveComparator::operator()(
    const std::string &left, const std::string &right) const noexcept {
  return strcasecmp(left.c_str(), right.c_str()) == 0;
}
