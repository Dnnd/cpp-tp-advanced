#ifndef PROCESS_WRAPPER_INCLUDE_TCP_EPOLLER_HPP_
#define PROCESS_WRAPPER_INCLUDE_TCP_EPOLLER_HPP_

#include <sys/epoll.h>
#include <vector>
#include "tcp/events.hpp"
namespace tcp {

// В gcc < 10 нет std::span
template <typename T> struct Span {
  T *data;
  const std::size_t size;
};

class Epoller {
public:
  explicit Epoller(int events_size);
  Epoller(const Epoller &) = delete;
  Epoller(Epoller &&other) noexcept;
  Epoller &operator=(const Epoller &other) = delete;
  Epoller &operator=(Epoller &&other) noexcept;
  void add(int fd, EventsSet events);

  void remove(int fd);
  void modify(int fd, EventsSet events);
  Span<epoll_event> wait(int timeout_musec);
  Span<epoll_event> wait();
  void swap(Epoller &other) noexcept;
  ~Epoller() noexcept;

private:
  void add(int fd, epoll_event &data);
  void modify(int fd, epoll_event &data);
  std::vector<epoll_event> events_;
  int epoll_fd_;
};
} // namespace tcp
#endif // PROCESS_WRAPPER_INCLUDE_TCP_EPOLLER_HPP_
