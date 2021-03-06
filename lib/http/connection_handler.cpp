#include "http/connection_handler.hpp"
#include "log/base_logger.hpp"

namespace {
constexpr size_t initial_buffer_size = 1024;
constexpr Coroutine::routine_t NO_COROUTINE = -1;
} // namespace

ConnectionHandler::ConnectionHandler(
    int f, EventsSet set, std::function<HttpResponse(HttpRequest &)> callback,
    log::BaseLogger &logger)
    : read_buff_(initial_buffer_size, '\0'), write_buff_(), fd_{f}, set_{set},
      last_activity_{std::chrono::milliseconds(0)},
      callback_(std::move(callback)), logger_{logger}, routine_id_{
                                                           NO_COROUTINE} {}

std::optional<HttpRequest> ConnectionHandler::readRequest() {
  HttpRequestParser parser{};

  std::size_t read_idx = 0;

  while (true) {
    auto ret =
        read(fd_, read_buff_.data() + read_idx, read_buff_.size() - read_idx);
    if (ret == -1 && errno == EAGAIN) {
      updateLastActivity();
      Coroutine::yield();
      if (isTimeout()) {
        throw TimeoutException("timeout readRequest");
      }
    } else if (ret == -1) {
      throw ReadingException("reading failed ");
    } else if (ret == 0) {
      return {};
    } else {
      read_idx += ret;
    }
    if (parser.parse(std::string_view{read_buff_.data(), read_idx + 1}) ==
        HttpRequestState::DONE) {
      break;
    }
    if (read_idx >= read_buff_.size()) {
      read_buff_.resize(read_buff_.size() * 2);
    }
  }
  return parser.buildRequest();
}

bool ConnectionHandler::writeResponse(HttpResponse &response) {
  size_t written = 0;
  write_buff_ = response.dump();
  while (written != write_buff_.size()) {
    auto ret =
        write(fd_, write_buff_.data() + written, write_buff_.size() - written);
    if (ret == -1 && errno == EAGAIN) {
      updateLastActivity();
      Coroutine::yield();
      if (isTimeout()) {
        throw TimeoutException("timeout writeResponse");
      }
    } else if (ret == -1 && errno == ECONNRESET) {
      return false;
    } else if (ret == -1 || ret == 0) {
      throw WritingException(std::string("writing failed ") +
                             std::strerror(errno));
    } else {
      written += ret;
    }
  }
  return true;
}
bool ConnectionHandler::shouldCloseAfterResponse(HttpRequest &req) {
  std::optional<std::string_view> connection = req.getHeader("Connection");
  if (req.getVersion() == HttpVersion::HTTP1_0) {
    return !(connection.has_value() && *connection == "Keep-Alive");
  } else if (req.getVersion() == HttpVersion::HTTP1_1) {
    return connection.has_value() && *connection == "close";
  }
  return true;
}
void ConnectionHandler::operator()() {
  if (isTimeout()) {
    return;
  }
  while (true) {
    try {
      std::optional<HttpRequest> request_opt = readRequest();
      if (!request_opt) {
        return;
      }
      HttpRequest &request = request_opt.value();
      bool shouldClose = shouldCloseAfterResponse(request);
      HttpResponse response = callback_(request);
      response.setVersion(request.getVersion());

      if (!shouldClose && request.getVersion() == HttpVersion::HTTP1_0) {
        response.addHeader("Connection", "keep-alive");
      }

      set_.sub(Events::IN);
      set_.add(Events::OUT);

      if (!writeResponse(response)) {
        return;
      }

      if (shouldClose) {
        return;
      }
      set_.sub(Events::OUT);
      set_.add(Events::IN);
    } catch (TimeoutException &e) {
      logger_.error(e.what());
      return;
    } catch (ParsingException &e) {
      logger_.error(e.what());
      set_.sub(Events::IN);
      set_.add(Events::OUT);
      writeResponse(HttpResponse().setStatusCode(e.getCode()));
      return;
    }
  }
}
EventsSet ConnectionHandler::getEventsSet() const { return set_; }
void ConnectionHandler::setTimeout() noexcept { timeout_ = true; }
bool ConnectionHandler::isTimeout() const noexcept { return timeout_; }
void ConnectionHandler::setLastActivity(
    std::chrono::time_point<std::chrono::steady_clock> last_activity) noexcept {
  last_activity_ = last_activity;
}
void ConnectionHandler::updateLastActivity() noexcept {
  setLastActivity(std::chrono::steady_clock::now());
}
std::chrono::time_point<std::chrono::steady_clock>
ConnectionHandler::getLastActivity() const noexcept {
  return last_activity_;
}

bool ConnectionHandler::resumeHandler() {
  if (routine_id_ != NO_COROUTINE) {
    return Coroutine::resume(routine_id_);
  }
  return true;
}

void ConnectionHandler::registerCoroutine() {
  routine_id_ = Coroutine::create(std::ref(*this));
}
ConnectionHandler::ConnectionHandler(ConnectionHandler &&other) noexcept
    : fd_{other.fd_}, last_activity_{other.last_activity_},
      read_buff_{std::move(other.read_buff_)}, logger_{other.logger_},
      write_buff_{std::move(other.write_buff_)}, callback_{other.callback_},
      routine_id_{other.routine_id_}, set_{other.set_} {
  other.routine_id_ = NO_COROUTINE;
  other.fd_ = -1;
}
ConnectionHandler::~ConnectionHandler() {
  if (fd_ != -1) {
    close(fd_);
    fd_ = -1;
  }
}
ConnectionHandler &
ConnectionHandler::operator=(ConnectionHandler &&other) noexcept {
  fd_ = other.fd_;
  last_activity_ = other.last_activity_;
  logger_ = other.logger_;
  write_buff_ = std::move(other.write_buff_);
  read_buff_ = std::move(other.read_buff_);
  callback_ = std::move(other.callback_);
  routine_id_ = other.routine_id_;
  set_ = other.set_;

  other.routine_id_ = NO_COROUTINE;
  other.fd_ = -1;
  return *this;
}
