#include "tcp/epoller.hpp"
#include "tcp/excpetions.hpp"
#include <cstring>
#include <unistd.h>

namespace tcp {

Epoller::Epoller(int events_size)
    : epoll_fd_{-1}, events_(events_size, epoll_event{}) {
  epoll_fd_ = epoll_create1(0);
  if (epoll_fd_ == -1) {
    throw Exception("unable to create epoll");
  }
}
Epoller::~Epoller() noexcept { ::close(epoll_fd_); }

void Epoller::add(int fd, epoll_event &ev) {
  if (-1 == epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &ev)) {
    throw EpollCtlException("unable to add event to epoll", EPOLL_CTL_MOD);
  }
}

void Epoller::modify(int fd, epoll_event &ev) {
  if (-1 == epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, fd, &ev)) {
    throw EpollCtlException("unable to modify event in epoll", EPOLL_CTL_MOD);
  }
}

void Epoller::add(int fd, EventsSet events) {
  epoll_event ev{};
  ev.events = events.data();
  ev.data.fd = fd;
  add(fd, ev);
}
void Epoller::modify(int fd, EventsSet events) {
  epoll_event ev{};
  ev.events = events.data();
  ev.data.fd = fd;
  modify(fd, ev);
}

void Epoller::remove(int fd) {
  if (-1 == epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, nullptr)) {
    throw EpollCtlException("unable to remove event from epoll", EPOLL_CTL_DEL);
  }
}
Span<epoll_event> Epoller::wait(int timeout_musec) {
  int triggered =
      epoll_wait(epoll_fd_, events_.data(), events_.size(), timeout_musec);
  if (triggered == -1 && errno == EINTR) {
    throw InterruptedException(std::strerror(errno));
  }
  if (triggered == -1) {
    throw Exception("failed on epoll_wait call");
  }
  return {.data = events_.data(), .size = static_cast<size_t>(triggered)};
}

Span<epoll_event> Epoller::wait() { return wait(-1); }
void Epoller::swap(Epoller &other) noexcept {
  std::swap(epoll_fd_, other.epoll_fd_);
  events_.swap(other.events_);
}
Epoller::Epoller(Epoller &&other) noexcept
    : epoll_fd_{other.epoll_fd_}, events_{std::move(other.events_)} {
  other.epoll_fd_ = -1;
}
Epoller &Epoller::operator=(Epoller &&other) noexcept {
  Epoller tmp{std::move(other)};
  swap(tmp);
  return *this;
};

} // namespace tcp