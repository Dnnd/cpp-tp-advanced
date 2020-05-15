#ifndef PROCESS_WRAPPER_LIB_TCP_EVENTS_HPP_
#define PROCESS_WRAPPER_LIB_TCP_EVENTS_HPP_

#include <cstdint>
#include <sys/epoll.h>
#include <utility>

enum class Events : uint32_t {
  IN = EPOLLIN,
  OUT = EPOLLOUT,
  ERR = EPOLLERR,
  PRI = EPOLLPRI,
  HUP = EPOLLHUP,
  RDHUP = EPOLLRDHUP,
  EXCLUSIVE = EPOLLEXCLUSIVE,
  EDGE = EPOLLET,
  ONESHOT = EPOLLONESHOT,
};

class EventsSet {
public:
  EventsSet(std::initializer_list<Events> events);
  explicit EventsSet(uint32_t events);
  EventsSet() = default;

  void sub(Events event) noexcept;
  void add(Events event) noexcept;

  [[nodiscard]] bool haveIntersection(EventsSet event) const noexcept;
  [[nodiscard]] bool contains(Events event) const noexcept;
  [[nodiscard]] EventsSet diff(EventsSet other) const noexcept;

  [[nodiscard]] uint32_t data() const noexcept;
  [[nodiscard]] bool empty() const noexcept;

  friend bool operator==(EventsSet lhs, EventsSet rhs);

private:
  uint32_t events_;
};

#endif // PROCESS_WRAPPER_LIB_TCP_EVENTS_HPP_
