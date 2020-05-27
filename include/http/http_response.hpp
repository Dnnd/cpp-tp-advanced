#ifndef PROCESS_WRAPPER_LIB_HTTP_HTTP_RESPONSE_HPP_
#define PROCESS_WRAPPER_LIB_HTTP_HTTP_RESPONSE_HPP_
#include "http/protocol.hpp"

class HttpResponse {
public:
  HttpResponse &setVersion(HttpVersion version) noexcept;
  HttpResponse &setStatusCode(HttpStatusCode code) noexcept {
    code_ = code;
    return *this;
  }
  HttpResponse &addHeader(std::string key, std::string value);

  HttpResponse &addHeaders(
      std::initializer_list<std::pair<std::string, std::string>> headers);

  HttpResponse &setBody(std::string body);

  [[nodiscard]] std::string dump() const;

private:
  HttpVersion version_;
  HttpStatusCode code_;
  std::vector<std::pair<std::string, std::string>> headers_;
  std::string body_;
};
#endif // PROCESS_WRAPPER_LIB_HTTP_HTTP_RESPONSE_HPP_
