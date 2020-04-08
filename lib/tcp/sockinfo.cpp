#include "tcp/sockinfo.hpp"
Sockinfo::Sockinfo(sockaddr_in addr) noexcept
    : address{ntohl(addr.sin_addr.s_addr)}, port{ntohs(addr.sin_port)} {}
