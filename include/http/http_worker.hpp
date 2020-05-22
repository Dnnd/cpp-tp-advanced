#ifndef PROCESS_WRAPPER_INCLUDE_HTTP_HTTP_WORKER_HPP_
#define PROCESS_WRAPPER_INCLUDE_HTTP_HTTP_WORKER_HPP_

#include "coroutine/coroutine.h"
#include "http/connection_handler.hpp"
#include "http/protocol.hpp"
#include "log/base_logger.hpp"
#include "parser.hpp"
#include "tcp/epoller.hpp"
#include "unistd.h"
#include <atomic>
#include <thread>

class HttpWorker {
public:
  HttpWorker(std::atomic_bool &stop, int server_fd,
             std::chrono::milliseconds timeout_threshold,
             std::function<HttpResponse(HttpRequest &)> callback,
             log::BaseLogger &logger);

  HttpWorker(const HttpWorker &other) = delete;
  HttpWorker(HttpWorker &&other) = delete;
  HttpWorker &operator=(HttpWorker &&other) noexcept = delete;
  HttpWorker &operator=(const HttpWorker &other) noexcept = delete;
  ~HttpWorker() noexcept;

private:
  void run_events_loop();
  void scheduleTimeoutHandler(
      std::chrono::time_point<std::chrono::steady_clock> now);

  void registerHandler(int client_fd);

  void resumeHandler(int client_fd, ConnectionHandler &handler);

  void handleEvents(tcp::Span<epoll_event> events);

  void registerClients();

  log::BaseLogger &logger_;
  std::function<HttpResponse(HttpRequest &)> callback_;
  std::chrono::milliseconds timeout_threshold_;
  std::atomic_bool &stop_;
  tcp::Epoller epoller_;
  std::unordered_map<int, ConnectionHandler> coroutines;
  std::thread thread_;
  int server_fd_;
};
#endif // PROCESS_WRAPPER_INCLUDE_HTTP_HTTP_WORKER_HPP_
