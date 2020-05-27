#include "http/http_response.hpp"
HttpResponse &HttpResponse::setVersion(HttpVersion version) noexcept {
  version_ = version;
  return *this;
}
HttpResponse &HttpResponse::addHeader(std::string key, std::string value) {
  headers_.emplace_back(std::move(key), std::move(value));
  return *this;
}
HttpResponse &HttpResponse::addHeaders(
    std::initializer_list<std::pair<std::string, std::string>> headers) {
  for (auto &&header : headers) {
    headers_.emplace_back(header);
  }
  return *this;
}
HttpResponse &HttpResponse::setBody(std::string body) {
  body_ = std::move(body);
  headers_.emplace_back("Content-Length", std::to_string(body_.size()));
  return *this;
}
std::string HttpResponse::dump() const {
  std::ostringstream ost;
  ost << version_ << ' ' << std::to_string(static_cast<uint16_t>(code_)) << ' '
      << reason_phrase_of(code_) << "\r\n";
  for (auto &&[header_key, header_value] : headers_) {
    ost << header_key << ':' << header_value << "\r\n";
  }
  ost << "\r\n";
  ost << body_;
  return ost.str();
}
