#include <stdexcept>
#include "process/pipe.hpp"

Pipe make_pipe() {
    int fd[2];
    if (pipe(fd) == -1) {
        throw std::runtime_error(std::strerror(errno));
    }
    return {Descriptor{fd[0]}, Descriptor{fd[1]}};
}
