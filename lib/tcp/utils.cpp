#include "tcp/utils.hpp"
#include "netdb.h"
#include "tcp/excpetions.hpp"
#include <cstring>
#include <netinet/in.h>
#include <string>
using namespace std::string_literals;

namespace tcp {
addrinfo_ptr resolve_ipv4_tcp(const std::string &hostname, std::uint16_t port) {
  addrinfo hints{};
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_ADDRCONFIG;
  addrinfo *response = nullptr;
  std::string portname = std::to_string(port);
  int err = getaddrinfo(hostname.c_str(), portname.c_str(), &hints, &response);
  if (err != 0) {
    throw Exception(std::strerror(errno));
  }
  return response;
}

sockaddr_in get_remote_sockaddr(int fd) {
  sockaddr_in sock{};
  socklen_t sockaddr_in_size = sizeof(sockaddr_in);
  auto *sockaddr_ptr = reinterpret_cast<sockaddr *>(&sock);
  if (getpeername(fd, sockaddr_ptr, &sockaddr_in_size) == -1) {
    throw Exception("unable to get remote socket address of fd: "s +
                    std::strerror(errno));
  }
  return sock;
}
sockaddr_in get_local_sockaddr(int fd) {
  sockaddr_in sock{};
  socklen_t sockaddr_in_size = sizeof(sockaddr_in);
  auto *sockaddr_ptr = reinterpret_cast<sockaddr *>(&sock);
  if (getsockname(fd, sockaddr_ptr, &sockaddr_in_size) == -1) {
    throw Exception("unable to get local socket address of fd"s +
                    std::strerror(errno));
  }
  return sock;
}
} // namespace tcp