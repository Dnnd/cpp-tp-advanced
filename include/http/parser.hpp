#ifndef PROCESS_WRAPPER_LIB_HTTP_PARSER_HPP_
#define PROCESS_WRAPPER_LIB_HTTP_PARSER_HPP_

#include "exceptions.hpp"
#include "http_request.hpp"
#include "protocol.hpp"
#include <optional>

enum class HttpRequestState {
  READING_REQUEST_LINE,
  READING_HEADERS,
  READING_BODY,
  DONE,
  ERROR,
};

class HttpRequestParser {
public:
  HttpRequestState parse(std::string_view message);

  HttpRequest buildRequest();

private:
  void parse_url(std::string_view message);

  void parse_query_parameters(std::string_view query);

  size_t parse_path(std::string_view message);

  void parse_version(std::string_view version);

  void parse_start_line(std::string_view message);

  size_t parse_request_token(std::string_view message);

  void parse_header(std::string_view header);

  void parse_headers(std::string_view headers);

private:
  size_t body_length_{0};
  size_t parsed_total_{0};
  HttpRequestState state_{HttpRequestState::READING_REQUEST_LINE};
  QueryParameters query_parameters_{};
  Headers headers_{};
  std::string_view body_{};
  std::string_view path_{};
  HttpVersion version_{HttpVersion::HTTP1_0};
  HttpMethod method_{HttpMethod::GET};
};

#endif // PROCESS_WRAPPER_LIB_HTTP_PARSER_HPP_
