#ifndef PROCESS_WRAPPER_INCLUDE_HTTP_PROTOCOL_HPP_
#define PROCESS_WRAPPER_INCLUDE_HTTP_PROTOCOL_HPP_

#include <regex>
#include <string>
#include <strings.h>
#include <unordered_map>

enum class HttpVersion : uint8_t {
  HTTP1_0,
  HTTP1_1,
};

enum class HttpStatusCode : uint16_t {
  CONTINUE = 100,
  SWITCHING_PROTOCOLS = 101,
  OK = 200,
  CREATED = 201,
  ACCEPTED = 202,
  NO_CONTENT = 204,
  PARTIAL_CONTENT = 206,
  MULTIPLE_CHOICES = 300,
  MOVED_PERMANENTLY = 301,
  FOUND = 302,
  SEE_OTHER = 303,
  USE_PROXY = 305,
  TEMPORARY_REDIRECT = 307,
  BAD_REQUEST = 400,
  UNAUTHORIZED = 401,
  PAYMENT_REQUIRED = 402,
  FORBIDDEN = 403,
  NOT_FOUND = 404,
  METHOD_NOT_ALLOWED = 405,
  NOT_ACCEPTABLE = 406,
  PROXY_AUTHENTICATION_REQUIRED = 407,
  REQUEST_TIMEOUT = 408,
  CONFLICT = 409,
  GONE = 410,
  LENGTH_REQUIRED = 411,
  PRECONDITION_FAILED = 412,
  REQUEST_ENTITY_TOO_LARGE = 413,
  REQUEST_URI_TOO_LARGE = 414,
  REQUESTED_RANGE_NOT_SATISFIABLE = 416,
  EXPECTATION_FAILED = 417,
  INTERNAL_SERVER_ERROR = 500,
  NOT_IMPLEMENTED = 501,
  BAD_GATEWAY = 502,
  SERVICE_UNABAILABLE = 503,
  GATEWAY_TIMEOUT = 504,
  HTTP_VERSION_NOT_SUPPORTED = 505
};

constexpr const char *reason_phrase_of(HttpStatusCode code) noexcept {
  switch (code) {
  case HttpStatusCode::OK:
    return "OK";
  case HttpStatusCode::BAD_REQUEST:
    return "Bad Request";
  case HttpStatusCode::NOT_FOUND:
    return "Not Found";
  case HttpStatusCode::INTERNAL_SERVER_ERROR:
    return "Internal Server Error";
  case HttpStatusCode::CONTINUE:
    return "Continue";
  case HttpStatusCode::SWITCHING_PROTOCOLS:
    return "Switching Protocols";
  case HttpStatusCode::CREATED:
    return "Created";
  case HttpStatusCode::ACCEPTED:
    return "Accepted";
  case HttpStatusCode::NO_CONTENT:
    return "No Content";
  case HttpStatusCode::PARTIAL_CONTENT:
    return "Partial Content";
  case HttpStatusCode::MULTIPLE_CHOICES:
    return "Multiple Choices";
  case HttpStatusCode::MOVED_PERMANENTLY:
    return "Moved Permanently";
  case HttpStatusCode::FOUND:
    return "Found";
  case HttpStatusCode::SEE_OTHER:
    return "See Other";
  case HttpStatusCode::USE_PROXY:
    return "User Proxy";
  case HttpStatusCode::TEMPORARY_REDIRECT:
    return "Temporary Redirect";
  case HttpStatusCode::UNAUTHORIZED:
    return "Unauthorized";
  case HttpStatusCode::PAYMENT_REQUIRED:
    return "Payment Required";
  case HttpStatusCode::FORBIDDEN:
    return "Forbidden";
  case HttpStatusCode::METHOD_NOT_ALLOWED:
    return "Method Not Allowed";
  case HttpStatusCode::NOT_ACCEPTABLE:
    return "Not Acceptable";
  case HttpStatusCode::PROXY_AUTHENTICATION_REQUIRED:
    return "Proxy Authentication Requried";
  case HttpStatusCode::REQUEST_TIMEOUT:
    return "Request Timeout";
  case HttpStatusCode::CONFLICT:
    return "Conflict";
  case HttpStatusCode::GONE:
    return "Gone";
  case HttpStatusCode::LENGTH_REQUIRED:
    return "Length Required";
  case HttpStatusCode::PRECONDITION_FAILED:
    return "Precondition Failed";
  case HttpStatusCode::REQUEST_ENTITY_TOO_LARGE:
    return "Request Entity Too Large";
  case HttpStatusCode::REQUEST_URI_TOO_LARGE:
    return "Request URI Too Large";
  case HttpStatusCode::REQUESTED_RANGE_NOT_SATISFIABLE:
    return "Requested Range Not Satisfiable";
  case HttpStatusCode::EXPECTATION_FAILED:
    return "Expectation Failed";
  case HttpStatusCode::NOT_IMPLEMENTED:
    return "Not Implemented";
  case HttpStatusCode::BAD_GATEWAY:
    return "Bad Gateway";
  case HttpStatusCode::SERVICE_UNABAILABLE:
    return "Service Unavailable";
  case HttpStatusCode::GATEWAY_TIMEOUT:
    return "Gateway Time-Out";
  case HttpStatusCode::HTTP_VERSION_NOT_SUPPORTED:
    return "HTTP Version Not Supported";
  }
  return "";
}

std::ostream &operator<<(std::ostream &os, const HttpVersion &version);

enum class HttpMethod : uint8_t { GET, POST, PUT, DELETE };

std::ostream &operator<<(std::ostream &os, const HttpMethod &version);

struct CaseInsensitiveComparator {
  bool operator()(const std::string_view &left,
                  const std::string_view &right) const noexcept;
};

using Headers =
    std::unordered_map<std::string, std::string, std::hash<std::string_view>,
                       CaseInsensitiveComparator>;
using QueryParameters =
    std::unordered_map<std::string, std::string, std::hash<std::string_view>,
                       CaseInsensitiveComparator>;
#endif // PROCESS_WRAPPER_INCLUDE_HTTP_PROTOCOL_HPP_
