#ifndef PROCESS_WRAPPER_INCLUDE_TCP_SERVER_HPP_
#define PROCESS_WRAPPER_INCLUDE_TCP_SERVER_HPP_
#include "connection.hpp"
#include <cstdint>
#include <string>

namespace tcp {
class Server {
public:
  Server(const std::string &hostname, uint16_t port);
  Server(const Server &other) = delete;
  Server(Server &&other) noexcept;
  Server &operator=(Server &&other) noexcept;
  void close();
  ~Server() noexcept;
  [[nodiscard]] Connection accept() const;
  void open(const std::string &hostname, uint16_t port);
  void setMaxConnect(int connmax);

  [[nodiscard]] bool isOpened() const noexcept;

  friend Connection::Connection(int fd);
  void swap(Server &other) noexcept;

private:
  int fd_;
  sockaddr_in socket_;
};
} // namespace tcp
#endif // PROCESS_WRAPPER_INCLUDE_TCP_SERVER_HPP_
