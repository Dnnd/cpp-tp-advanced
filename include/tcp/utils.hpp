#ifndef PROCESS_WRAPPER_INCLUDE_TCP_UTILS_HPP_
#define PROCESS_WRAPPER_INCLUDE_TCP_UTILS_HPP_

#include "netdb.h"
#include <memory>
#include <netinet/in.h>

namespace tcp {
struct AddrinfoDeleter {
  void operator()(addrinfo *info);
};

using addrinfo_raii = std::unique_ptr<addrinfo, AddrinfoDeleter>;

addrinfo_raii resolve_ipv4_tcp(const std::string &hostname, std::uint16_t port);

sockaddr_in get_remote_sockaddr(int fd);

sockaddr_in get_local_sockaddr(int fd);
} // namespace tcp
#endif // PROCESS_WRAPPER_INCLUDE_TCP_UTILS_HPP_
