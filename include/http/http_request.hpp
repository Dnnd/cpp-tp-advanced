#ifndef PROCESS_WRAPPER_INCLUDE_HTTP_HTTP_REQUEST_HPP_
#define PROCESS_WRAPPER_INCLUDE_HTTP_HTTP_REQUEST_HPP_
#include "http/protocol.hpp"

class HttpRequest {
public:
  HttpRequest(Headers headers, QueryParameters query_parameters,
              std::string_view body, std::string_view path, HttpVersion version,
              HttpMethod method);

  [[nodiscard]] std::string_view getBody() const noexcept;
  [[nodiscard]] Headers &getHeadres() noexcept;
  [[nodiscard]] const Headers &getHeadres() const noexcept;
  [[nodiscard]] const QueryParameters &getQueryParameters() const noexcept;
  [[nodiscard]] QueryParameters &getQueryParameters() noexcept;
  [[nodiscard]] std::optional<std::string_view>
  getHeader(const std::string &header_name) const noexcept;

  [[nodiscard]] std::optional<std::string_view>
  getQueryParameter(const std::string &param_name) const noexcept;

  [[nodiscard]] std::string_view getResourceTarget() const;
  [[nodiscard]] HttpVersion getVersion() const noexcept;
  [[nodiscard]] HttpMethod getMethod() const noexcept;

private:
  Headers headers_;
  QueryParameters query_parameters_;
  std::string body_;
  std::string path_;
  HttpVersion version_;
  HttpMethod method_;
};
#endif // PROCESS_WRAPPER_INCLUDE_HTTP_HTTP_REQUEST_HPP_
