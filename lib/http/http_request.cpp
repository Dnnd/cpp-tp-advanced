#include "http/http_request.hpp"
HttpRequest::HttpRequest(Headers headers, QueryParameters query_parameters,
                         std::string_view body, std::string_view path,
                         HttpVersion version, HttpMethod method)
    : headers_{std::move(headers)}, query_parameters_{std::move(
                                        query_parameters)},
      body_{body}, path_{path}, version_{version}, method_{method} {}
std::string_view HttpRequest::getBody() const noexcept { return body_; }
Headers &HttpRequest::getHeadres() noexcept { return headers_; }
const Headers &HttpRequest::getHeadres() const noexcept { return headers_; }
const QueryParameters &HttpRequest::getQueryParameters() const noexcept {
  return query_parameters_;
}
QueryParameters &HttpRequest::getQueryParameters() noexcept {
  return query_parameters_;
}
std::optional<std::string_view>
HttpRequest::getHeader(const std::string &header_name) const noexcept {
  if (auto it = headers_.find(header_name); it != headers_.end()) {
    return it->second;
  }
  return {};
}
std::optional<std::string_view>
HttpRequest::getQueryParameter(const std::string &param_name) const noexcept {
  if (auto it = query_parameters_.find(param_name); it != headers_.end()) {
    return it->second;
  }
  return {};
}
std::string_view HttpRequest::getResourceTarget() const { return path_; }
HttpVersion HttpRequest::getVersion() const noexcept { return version_; }
HttpMethod HttpRequest::getMethod() const noexcept { return method_; }
