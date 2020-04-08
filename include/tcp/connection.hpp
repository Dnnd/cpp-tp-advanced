#ifndef PROCESS_WRAPPER_INCLUDE_TCP_CONNECTION_HPP_
#define PROCESS_WRAPPER_INCLUDE_TCP_CONNECTION_HPP_
#include "sockinfo.hpp"
#include <cstdint>
#include <netinet/in.h>
#include <string>
#include <unistd.h>

namespace tcp {

class Connection {
public:
  Connection(const std::string &hostname, std::uint16_t port,
             time_t timeout_sec = 0);
  Connection(const Connection &other) = delete;
  Connection &operator=(const Connection &other) = delete;
  Connection(Connection &&other) noexcept;
  Connection &operator=(Connection &&other) noexcept;
  std::size_t read(void *data, std::size_t len);

  std::size_t write(const void *data, std::size_t len);

  void readExact(void *data, std::size_t len);

  void writeExact(const void *data, std::size_t len);

  void connect(const std::string &hostname, std::uint16_t port,
               time_t timeout_sec = 0);

  void setTimeout(time_t timeout_sec);

  void close();
  ~Connection() noexcept;

  [[nodiscard]] bool isOpened() const noexcept;

  friend class Server;

  void swap(Connection &other) noexcept;

private:
  explicit Connection(int fd);
  void setTimeout(int fd, time_t timeout_sec) const;
  int fd_;
  Sockinfo local_socket;
  Sockinfo remote_socket;
};
} // namespace tcp
#endif // PROCESS_WRAPPER_INCLUDE_TCP_CONNECTION_HPP_
