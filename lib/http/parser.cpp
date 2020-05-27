#include "http/parser.hpp"
#include <charconv>

HttpRequestState HttpRequestParser::parse(std::string_view message) {

  message.remove_prefix(parsed_total_);
  if (state_ == HttpRequestState::READING_REQUEST_LINE) {
    if (size_t req_line_idx = message.find("\r\n");
        req_line_idx != std::string::npos) {
      parse_start_line(message.substr(0, req_line_idx));
      state_ = HttpRequestState::READING_HEADERS;
      parsed_total_ = req_line_idx + 2;
      message.remove_prefix(parsed_total_);
    }
  }

  if (state_ == HttpRequestState::READING_HEADERS) {
    size_t headers_end = message.find("\r\n\r\n");
    if (headers_end == std::string::npos) {
      return state_;
    }
    parse_headers(message.substr(0, headers_end + 2));

    if (method_ == HttpMethod::GET || method_ == HttpMethod::DELETE) {
      state_ = HttpRequestState::DONE;
      return state_;
    }

    auto it = headers_.find("Content-Length");
    if (it == headers_.end()) {
      state_ = HttpRequestState::DONE;
      return state_;
    }
    std::string_view content_length = it->second;
    body_length_ = std::stoull(content_length.data());
    state_ = HttpRequestState::READING_BODY;
    parsed_total_ += headers_end + 4;
    message.remove_prefix(headers_end + 4);
  }

  if (state_ == HttpRequestState::READING_BODY) {
    if (message.length() >= body_length_) {
      body_ = message.substr(0, body_length_);
      state_ = HttpRequestState::DONE;
      return state_;
    }
  }
  return state_;
}
HttpRequest HttpRequestParser::buildRequest() {
  return HttpRequest{std::move(headers_),
                     std::move(query_parameters_),
                     body_,
                     path_,
                     version_,
                     method_};
}
void HttpRequestParser::parse_url(std::string_view message) {
  size_t query_begin = message.find('?');

  if (query_begin != std::string::npos &&
      message.length() > (query_begin + 1)) {
    parse_query_parameters(message.substr(query_begin + 1));
  }

  path_ = message.substr(0, query_begin);

  if (path_.length() == 0) {
    state_ = HttpRequestState::ERROR;
    throw ParsingException("invalid resource target",
                           HttpStatusCode::BAD_REQUEST);
  }
}
void HttpRequestParser::parse_query_parameters(std::string_view query) {
  while (true) {
    size_t param_end = query.find('&');
    if (param_end == std::string::npos) {
      param_end = query.size();
    }
    std::string_view param{query.begin(), param_end};
    size_t param_delim = param.find('=');
    if (param_delim == std::string::npos) {
      throw ParsingException("invalid query parameter",
                             HttpStatusCode::BAD_REQUEST);
    }
    query_parameters_.insert({std::string(param.substr(0, param_delim)),
                              std::string{param.substr(param_delim + 1)}});
    if (query.size() == param_end) {
      return;
    }
    query.remove_prefix(param_end + 1);
  }
}
size_t HttpRequestParser::parse_path(std::string_view message) {
  std::size_t path_end = message.find(' ');
  if (path_end == std::string::npos) {
    throw ParsingException("expected space after resource target",
                           HttpStatusCode::BAD_REQUEST);
  }
  parse_url(message.substr(0, path_end));
  return path_end + 1;
}
void HttpRequestParser::parse_version(std::string_view version) {
  if (version == "HTTP/1.1") {
    version_ = HttpVersion::HTTP1_1;
  } else if (version == "HTTP/1.0") {
    version_ = HttpVersion::HTTP1_0;
  } else {
    throw ParsingException("unsupported HTTP version",
                           HttpStatusCode::HTTP_VERSION_NOT_SUPPORTED);
  }
}
void HttpRequestParser::parse_start_line(std::string_view message) {
  size_t token_end = parse_request_token(message);
  message.remove_prefix(token_end);
  size_t path_end = parse_path(message);
  message.remove_prefix(path_end);
  parse_version(message);
}
size_t HttpRequestParser::parse_request_token(std::string_view message) {
  std::size_t after_token = message.find(' ');
  if (after_token == std::string::npos) {
    throw ParsingException("expected single space after request token",
                           HttpStatusCode::BAD_REQUEST);
  }
  if (message.starts_with("GET")) {
    method_ = HttpMethod::GET;
    return after_token + 1;
  } else if (message.starts_with("POST")) {
    method_ = HttpMethod::POST;
    return after_token + 1;
  } else if (message.starts_with("DELETE")) {
    method_ = HttpMethod::DELETE;
    return after_token + 1;
  } else if (message.starts_with("PUT")) {
    method_ = HttpMethod::PUT;
    return after_token + 1;
  }
  throw ParsingException("invalid request token", HttpStatusCode::BAD_REQUEST);
}

void HttpRequestParser::parse_header(std::string_view header) {
  size_t header_delim = header.find(':');
  if (header_delim == std::string::npos || header_delim >= header.size()) {

    throw ParsingException("invalid header", HttpStatusCode::BAD_REQUEST);
  }
  size_t header_name_end = header_delim;
  if (header[header_delim + 1] == ' ') {
    ++header_delim;
  }
  size_t header_end =
      (header.back() == ' ') ? header.size() - 1 : header.size();
  size_t header_value_begin = header_delim + 1;
  headers_.insert({std::string(header.substr(0, header_name_end)),
                   std::string(header.substr(header_value_begin, header_end))});
}

void HttpRequestParser::parse_headers(std::string_view headers) {
  while (!headers.empty()) {
    size_t header_len = headers.find("\r\n");
    parse_header(headers.substr(0, header_len));
    headers.remove_prefix(header_len + 2);
  }
}
