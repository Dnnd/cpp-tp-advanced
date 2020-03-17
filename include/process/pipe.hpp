#ifndef PROCESS_WRAPPER_PIPE_HPP
#define PROCESS_WRAPPER_PIPE_HPP

#include "descriptor.hpp"

struct Pipe {
  Descriptor reader;
  Descriptor writer;
};

[[nodiscard]] Pipe make_pipe();

#endif // PROCESS_WRAPPER_PIPE_HPP
