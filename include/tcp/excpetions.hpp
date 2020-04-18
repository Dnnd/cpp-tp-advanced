#ifndef PROCESS_WRAPPER_INCLUDE_TCP_EXCPETIONS_HPP_
#define PROCESS_WRAPPER_INCLUDE_TCP_EXCPETIONS_HPP_
#include "tcp/sockinfo.hpp"
#include <exception>
#include <netinet/in.h>
#include <string>

namespace tcp {
class Exception : public std::exception {
public:
  explicit Exception(const std::string &message);
  [[nodiscard]] const char *what() const noexcept override;

private:
  std::string message_;
};

class SocketOpeningException : Exception {
public:
  SocketOpeningException(const std::string &message,
                         const std::string &hostname, uint16_t port);

  [[nodiscard]] const std::string &getHostname() const;
  [[nodiscard]] uint16_t getPort() const;

private:
  uint16_t port_;
  std::string hostname_;
};

class SocketException : public Exception {
public:
  SocketException(const std::string &message, Sockinfo local_socket,
                  Sockinfo remote_socket);
  [[nodiscard]] Sockinfo getLocal() const;
  [[nodiscard]] Sockinfo getRemote() const;

private:
  Sockinfo local_;
  Sockinfo remote_;
};

class ServerSocketException : public Exception {
public:
  explicit ServerSocketException(const std::string &message,
                                 Sockinfo server_socket_);

  [[nodiscard]] Sockinfo getServerSocket() const noexcept;

private:
  Sockinfo server_socket_;
};

class EpollCtlException : public Exception {
public:
  explicit EpollCtlException(const std::string &message, int epoll_ctl_op);

  [[nodiscard]] int getEpollCtlOp() const noexcept;

private:
  int epoll_ctl_op_;
};

class InterruptedException : public Exception {
public:
  explicit InterruptedException(const std::string &message);
};

} // namespace tcp
#endif // PROCESS_WRAPPER_INCLUDE_TCP_EXCPETIONS_HPP_
