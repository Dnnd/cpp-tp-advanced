#include "process/pipe.hpp"
#include <cstring>
#include <stdexcept>
#include <string>
#include <unistd.h>

using namespace std::string_literals;

[[nodiscard]] Pipe make_pipe() {
  int fd[2];
  if (pipe(fd) == -1) {
    throw std::runtime_error("pipe() syscall failed in make_pipe(): "s +
                             std::strerror(errno));
  }
  return {Descriptor{fd[0]}, Descriptor{fd[1]}};
}
