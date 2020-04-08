#ifndef PROCESS_WRAPPER_INCLUDE_TCP_SOCKINFO_HPP_
#define PROCESS_WRAPPER_INCLUDE_TCP_SOCKINFO_HPP_
#include <cstdint>
#include <netinet/in.h>
#include <stdint-gcc.h>

struct Sockinfo {
  explicit Sockinfo(sockaddr_in) noexcept;
  Sockinfo() = default;
  std::uint32_t address;
  std::uint16_t port;
};

#endif // PROCESS_WRAPPER_INCLUDE_TCP_SOCKINFO_HPP_
