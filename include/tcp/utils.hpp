#ifndef PROCESS_WRAPPER_INCLUDE_TCP_UTILS_HPP_
#define PROCESS_WRAPPER_INCLUDE_TCP_UTILS_HPP_

#include "netdb.h"
#include <memory>
#include <netinet/in.h>

namespace tcp {

template <class T, auto Deleter,
          class Base = std::unique_ptr<T, decltype(Deleter)>>
struct SmartPtr : Base {
  SmartPtr() : Base(nullptr, Deleter) {}
  SmartPtr(T *v) : Base(v, Deleter) {}
  SmartPtr &operator=(T *v) {
    Base::reset(v);
    return *this;
  }
  operator T *() const { return Base::get(); }
};

using addrinfo_ptr = SmartPtr<addrinfo, freeaddrinfo>;

addrinfo_ptr resolve_ipv4_tcp(const std::string &hostname, std::uint16_t port);

sockaddr_in get_remote_sockaddr(int fd);

sockaddr_in get_local_sockaddr(int fd);
} // namespace tcp
#endif // PROCESS_WRAPPER_INCLUDE_TCP_UTILS_HPP_
