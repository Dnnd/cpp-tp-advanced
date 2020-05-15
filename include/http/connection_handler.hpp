#ifndef PROCESS_WRAPPER_INCLUDE_HTTP_CONNECTION_HANDLER_HPP_
#define PROCESS_WRAPPER_INCLUDE_HTTP_CONNECTION_HANDLER_HPP_

#include "coroutine/coroutine.h"
#include "http/http_request.hpp"
#include "http/http_response.hpp"
#include "http/parser.hpp"
#include "log/base_logger.hpp"
#include "parser.hpp"
#include "tcp/events.hpp"
#include <functional>
#include <unistd.h>

class ConnectionHandler {
public:
  ConnectionHandler(int f, EventsSet set,
                    std::function<HttpResponse(HttpRequest &)> callback,
                    log::BaseLogger &logger);

  std::optional<HttpRequest> readRequest();

  void writeResponse(HttpResponse &response);

  static bool shouldCloseAfterResponse(HttpRequest &req);

  void operator()();

  [[nodiscard]] EventsSet getEventsSet() const;

  void setTimeout() noexcept;

  [[nodiscard]] bool isTimeout() const noexcept;

  void setLastActivity(std::chrono::time_point<std::chrono::steady_clock>
                           last_activity) noexcept;

  void updateLastActivity() noexcept;
  [[nodiscard]] std::chrono::time_point<std::chrono::steady_clock>
  getLastActivity() const noexcept;

  ~ConnectionHandler() = default;

private:
  std::chrono::time_point<std::chrono::steady_clock> last_activity_;
  bool timeout_ = false;
  EventsSet set_;
  std::string read_buff_;
  std::string write_buff_;
  int fd_;
  log::BaseLogger &logger_;
  std::function<HttpResponse(HttpRequest &)> callback_;
};
#endif // PROCESS_WRAPPER_INCLUDE_HTTP_CONNECTION_HANDLER_HPP_
