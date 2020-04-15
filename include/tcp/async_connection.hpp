#ifndef PROCESS_WRAPPER_INCLUDE_TCP_ASYNC_CONNECTION_HPP_
#define PROCESS_WRAPPER_INCLUDE_TCP_ASYNC_CONNECTION_HPP_

#include "tcp/events.hpp"
#include "tcp/sockinfo.hpp"
#include <memory>
#include <optional>
#include <utility>
#include <vector>

namespace tcp {

class NonBlockResult {
public:
  explicit NonBlockResult(std::size_t res);
  NonBlockResult();

  [[nodiscard]] bool isEOF() const noexcept;
  [[nodiscard]] bool again() const;
  [[nodiscard]] size_t bytes() const;

private:
  std::optional<std::size_t> result_;
};

class AsyncConnection {
public:
  ~AsyncConnection() noexcept;
  AsyncConnection(const AsyncConnection &) = delete;
  AsyncConnection(AsyncConnection &&) noexcept;
  AsyncConnection &operator=(const AsyncConnection &other) = delete;
  AsyncConnection &operator=(AsyncConnection &&other) noexcept;
  void swap(AsyncConnection &other) noexcept;

  NonBlockResult read(char *data, std::size_t size);
  NonBlockResult write(const char *data, size_t size);

  void close();

  [[nodiscard]] bool isOpened() const noexcept;

  [[nodiscard]] bool triggersOn(EventsSet event) const noexcept;

  void setInterestsSet(EventsSet new_set) noexcept;

  [[nodiscard]] const EventsSet &getInterestSet() const noexcept;
  EventsSet &getInterestSet() noexcept;

  friend class AsyncServer;

private:
  explicit AsyncConnection(int fd);

  int fd_;
  Sockinfo local_socket_{};
  Sockinfo remote_socket_{};
  EventsSet interest_set_;
};

}; // namespace tcp

#endif // PROCESS_WRAPPER_INCLUDE_TCP_ASYNC_CONNECTION_HPP_
