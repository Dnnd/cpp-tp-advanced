#ifndef PROCESS_WRAPPER_PIPE_HPP
#define PROCESS_WRAPPER_PIPE_HPP

#include "descriptor.hpp"
#include <utility>

using Pipe = std::pair<Descriptor, Descriptor>;

[[nodiscard]] Pipe make_pipe();

#endif // PROCESS_WRAPPER_PIPE_HPP
