#ifndef PROCESS_WRAPPER_INCLUDE_TCP_ASYNC_SERVER_HPP_
#define PROCESS_WRAPPER_INCLUDE_TCP_ASYNC_SERVER_HPP_
#include "tcp/async_connection.hpp"
#include "tcp/epoller.hpp"
#include "tcp/sockinfo.hpp"
#include <functional>
#include <memory>
#include <string>

namespace tcp {
class AsyncServer {
public:
  using AsyncConnectionPtr = std::shared_ptr<AsyncConnection>;
  using Callback =
      std::function<void(const AsyncConnectionPtr &, EventsSet events)>;

  AsyncServer(const std::string &hostname, uint16_t port, Callback handler,
              EventsSet default_interests_set = {Events::IN, Events::OUT},
              int timeout = -1);
  AsyncServer(const AsyncServer &other) = delete;
  AsyncServer(AsyncServer &&other) noexcept;
  AsyncServer &operator=(AsyncServer &&other) noexcept;
  void close();
  ~AsyncServer() noexcept;
  void accept();
  void open(const std::string &hostname, uint16_t port,
            EventsSet default_interests_set);
  void setMaxConnect(int connmax);

  [[nodiscard]] bool isOpened() const noexcept;

  void swap(AsyncServer &other) noexcept;

  [[nodiscard]] Sockinfo getServerSocketInfo() const noexcept;

private:
  void acceptConnections();
  void handleEvent(const epoll_event &event);
  void closeConnection(int fd);
  void registerConnection(int fd);
  AsyncConnectionPtr getConnection(int fd) const;

  Epoller poller_;
  int fd_;
  Sockinfo socket_;
  Callback handler_;
  std::unordered_map<int, AsyncConnectionPtr> connections_;
  EventsSet default_interests_set_;
  int timeout_;
};

};     // namespace tcp
#endif // PROCESS_WRAPPER_INCLUDE_TCP_ASYNC_SERVER_HPP_
