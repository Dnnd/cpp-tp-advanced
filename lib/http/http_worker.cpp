#include "http/http_worker.hpp"
#include "log/base_logger.hpp"

namespace {
constexpr int TIMER_TICK{250};
}

HttpWorker::HttpWorker(tcp::Epoller &epoller,
                       std::chrono::milliseconds timeout_threshold,
                       std::function<HttpResponse(HttpRequest &)> callback,
                       log::BaseLogger &logger)
    : callback_{std::move(callback)}, epoller_{epoller},
      timeout_threshold_{timeout_threshold}, logger_{logger} {}
void HttpWorker::operator()() {

  std::chrono::milliseconds time_passed;

  while (!stop_) {
    tcp::Span<epoll_event> events{};

    auto before = std::chrono::steady_clock::now();
    try {
      auto events_or_timeout = epoller_.wait_with_timeout(TIMER_TICK);
      if (!events_or_timeout.has_value()) {
        scheduleTimeoutHandler(std::chrono::steady_clock::now());
        continue;
      } else {
        events = events_or_timeout.value();
      }
    } catch (tcp::InterruptedException &ex) {
      continue;
    }
    handleEvents(events);
    auto now = std::chrono::steady_clock::now();
    time_passed +=
        std::chrono::duration_cast<std::chrono::milliseconds>(now - before);
    if (time_passed >= std::chrono::milliseconds(TIMER_TICK)) {
      scheduleTimeoutHandler(now);
      time_passed = std::chrono::milliseconds(0);
    }
  }
}
void HttpWorker::scheduleTimeoutHandler(
    std::chrono::time_point<std::chrono::steady_clock> now) {
  for (auto it = coroutines.begin(); it != coroutines.end();) {
    auto &&[fd, handler] = *it;
    auto &&[context, coroutine_id] = handler;
    if (context.getLastActivity() - now >= timeout_threshold_) {
      context.setTimeout();
      Coroutine::resume(coroutine_id);
      it = coroutines.erase(it);
    } else {
      ++it;
    }
  }
}
void HttpWorker::registerHandler(int client_fd) {
  auto handler =
      ConnectionHandler(client_fd, {Events::IN, Events::EDGE, Events::ONESHOT},
                        callback_, logger_);
  auto coro_id = Coroutine::create(handler);
  auto &&[elem_it, ok] =
      coroutines.try_emplace(client_fd, std::move(handler), coro_id);
  resumeHandler(client_fd, coro_id, elem_it->second.first);
}
void HttpWorker::resumeHandler(int client_fd, Coroutine::routine_t coro_id,
                               ConnectionHandler &handler) {
  try {
    if (!Coroutine::resume(coro_id)) {
      close(client_fd);
      coroutines.erase(client_fd);
    } else {
      epoller_.modify(client_fd, handler.getEventsSet());
    }
  } catch (tcp::Exception &e) {
    close(client_fd);
    coroutines.erase(client_fd);
    logger_.error(e.what());
  } catch (std::exception &e) {
    close(client_fd);
    coroutines.erase(client_fd);
    logger_.error(e.what());
  }
}
void HttpWorker::handleEvents(tcp::Span<epoll_event> events) {
  for (epoll_event *event = events.data; event != events.data + events.size;
       ++event) {
    int client_fd = event->data.fd;

    auto it = coroutines.find(client_fd);
    if (it == coroutines.end()) {
      registerHandler(client_fd);
    } else {
      auto &&[conn_handler, coro_id] = it->second;
      resumeHandler(client_fd, coro_id, conn_handler);
    }
  }
}
