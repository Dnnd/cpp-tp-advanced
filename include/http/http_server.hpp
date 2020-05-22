#ifndef PROCESS_WRAPPER_INCLUDE_HTTP_SERVER_HPP_
#define PROCESS_WRAPPER_INCLUDE_HTTP_SERVER_HPP_

#include "http_request.hpp"
#include "http_response.hpp"
#include "log/base_logger.hpp"
#include "tcp/epoller.hpp"
#include <atomic>
#include <chrono>
#include <csignal>
#include <string>

class HttpServer {
public:
  HttpServer(const std::string &hostname, uint16_t port,
             size_t thread_pool_size, std::chrono::milliseconds timeout,
             std::unique_ptr<log::BaseLogger>);

  virtual HttpResponse serveRequest(HttpRequest &request) = 0;

  void run(bool enable_graceful_shutdown);

  virtual ~HttpServer() noexcept;

private:
  int fd_;
  std::atomic_bool closed_;
  std::chrono::milliseconds timeout_;
  size_t thread_pool_size_;
  std::unique_ptr<log::BaseLogger> logger_;
};

#endif // PROCESS_WRAPPER_INCLUDE_HTTP_SERVER_HPP_
