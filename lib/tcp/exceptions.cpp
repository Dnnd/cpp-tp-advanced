#include "tcp/excpetions.hpp"
#include "tcp/sockinfo.hpp"
namespace tcp {
const char *Exception::what() const noexcept { return message_.c_str(); }

Exception::Exception(const std::string &message) : message_{message} {}

SocketOpeningException::SocketOpeningException(const std::string &message,
                                               const std::string &hostname,
                                               uint16_t port)
    : Exception{message}, hostname_{hostname}, port_{port} {}

const std::string &SocketOpeningException::getHostname() const {
  return hostname_;
}

uint16_t SocketOpeningException::getPort() const { return port_; }

SocketException::SocketException(const std::string &message,
                                 Sockinfo local_socket, Sockinfo remote_socket)
    : Exception{message}, local_{local_socket}, remote_{remote_socket} {}

Sockinfo SocketException::getLocal() const { return local_; }
Sockinfo SocketException::getRemote() const { return remote_; }

ServerSocketException::ServerSocketException(const std::string &message,
                                             Sockinfo server_socket)
    : Exception{message}, server_socket_{server_socket} {}

Sockinfo ServerSocketException::getServerSocket() const noexcept {
  return server_socket_;
}
EpollCtlException::EpollCtlException(const std::string &message,
                                     int epoll_ctl_op)
    : Exception{message}, epoll_ctl_op_{epoll_ctl_op} {}
int EpollCtlException::getEpollCtlOp() const noexcept { return epoll_ctl_op_; }

InterruptedException::InterruptedException(const std::string &message)
    : Exception{message} {}

} // namespace tcp