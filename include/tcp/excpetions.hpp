#ifndef PROCESS_WRAPPER_INCLUDE_TCP_EXCPETIONS_HPP_
#define PROCESS_WRAPPER_INCLUDE_TCP_EXCPETIONS_HPP_
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
  SocketException(const std::string &message, sockaddr_in local_socket,
                  sockaddr_in remote_socket);
  [[nodiscard]] sockaddr_in getLocal() const;
  [[nodiscard]] sockaddr_in getRemote() const;

private:
  sockaddr_in local_;
  sockaddr_in remote_;
};

class ServerSocketException : public Exception {
public:
  explicit ServerSocketException(const std::string &message,
                                 sockaddr_in server_socket_);

  [[nodiscard]] sockaddr_in getServerSocket() const noexcept;

private:
  sockaddr_in server_socket_;
};
} // namespace tcp
#endif // PROCESS_WRAPPER_INCLUDE_TCP_EXCPETIONS_HPP_
