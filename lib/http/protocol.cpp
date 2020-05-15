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
    const std::string_view &left,
    const std::string_view &right) const noexcept {
  if (left.size() < right.size()) {
    return strncasecmp(left.data(), right.data(), left.size()) == 0;
  } else {
    return strncasecmp(right.data(), left.data(), right.size()) == 0;
  }
}
