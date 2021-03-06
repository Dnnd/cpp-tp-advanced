#include "kvstore/descriptor.hpp"
#include <unistd.h>

Descriptor::Descriptor(int fd) : fd_{fd} {}

int Descriptor::get() const noexcept { return fd_; }
Descriptor::~Descriptor() noexcept {
  if (fd_ == -1) {
    close(fd_);
  }
}
