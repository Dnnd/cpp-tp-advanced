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

  void run();

  void acceptClients();

  static void stop();

  static void enableGracefulShutdown();

  virtual ~HttpServer() noexcept;

private:
  static void _stop(int sig, siginfo_t *siginfo, void *context) { stop(); }
  tcp::Epoller accept_poller_;
  int fd_;
  static std::atomic_bool closed_;
  std::vector<tcp::Epoller> pollers_pool_;
  std::chrono::milliseconds timeout_;
  size_t thread_pool_size_;
  std::unique_ptr<log::BaseLogger> logger_;
};

#endif // PROCESS_WRAPPER_INCLUDE_HTTP_SERVER_HPP_
