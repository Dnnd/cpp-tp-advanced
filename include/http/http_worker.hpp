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

class HttpWorker {
public:
  using Handler = std::pair<ConnectionHandler, Coroutine::routine_t>;

  HttpWorker(tcp::Epoller &epoller, std::chrono::milliseconds timeout_threshold,
             std::function<HttpResponse(HttpRequest &)> callback,
             log::BaseLogger &logger);

  void operator()();

  void scheduleTimeoutHandler(
      std::chrono::time_point<std::chrono::steady_clock> now);

  void registerHandler(int client_fd);

  void resumeHandler(int client_fd, Coroutine::routine_t coro_id,
                     ConnectionHandler &handler);

  void handleEvents(tcp::Span<epoll_event> events);

private:
  log::BaseLogger &logger_;
  std::function<HttpResponse(HttpRequest &)> callback_;
  std::chrono::milliseconds timeout_threshold_;
  std::atomic<bool> stop_{false};
  tcp::Epoller &epoller_;
  std::unordered_map<int, Handler> coroutines;
};
#endif // PROCESS_WRAPPER_INCLUDE_HTTP_HTTP_WORKER_HPP_
