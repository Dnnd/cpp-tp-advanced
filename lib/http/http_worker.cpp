#include "http/http_worker.hpp"
#include "log/base_logger.hpp"
#include <http/utils.hpp>
using namespace std::string_literals;
namespace {
constexpr int TIMER_TICK{250};
}

HttpWorker::HttpWorker(tcp::Epoller &epoller,
                       std::chrono::milliseconds timeout_threshold,
                       std::function<HttpResponse(HttpRequest &)> callback,
                       log::BaseLogger &logger)
    : callback_{std::move(callback)}, epoller_{epoller},
      timeout_threshold_{timeout_threshold}, logger_{logger}, thread_{} {
  thread_ = std::thread{[this] { run_events_loop(); }};
}

void HttpWorker::run_events_loop() {
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
    if (now - handler.getLastActivity() >= timeout_threshold_) {
      handler.setTimeout();
      try {
        handler.resumeHandler();
      } catch (std::exception &e) {
        logger_.error("coroutine throwed exception on closing :"s + e.what());
      }
      close(it->first);
      it = coroutines.erase(it);
    } else {
      ++it;
    }
  }
}
void HttpWorker::registerHandler(int client_fd) {
  auto handler = ConnectionHandler(client_fd, {Events::IN}, callback_, logger_);

  auto &&[elem_it, ok] = coroutines.emplace(client_fd, std::move(handler));
  auto &inserted_handler = elem_it->second;
  inserted_handler.registerCoroutine();
  resumeHandler(client_fd, inserted_handler);
}

void HttpWorker::resumeHandler(int client_fd, ConnectionHandler &handler) {
  try {
    EventsSet events_before = handler.getEventsSet();
    if (!handler.resumeHandler()) {
      close(client_fd);
      coroutines.erase(client_fd);
    } else if (events_before != handler.getEventsSet()) {
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
      auto &conn_handler = it->second;
      resumeHandler(client_fd, conn_handler);
    }
  }
}
HttpWorker::~HttpWorker() noexcept {
  try {
    if (thread_.joinable()) {
      thread_.join();
    }
  } catch (std::system_error &e) {
  }
}
