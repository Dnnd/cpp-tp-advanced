#ifndef PROCESS_WRAPPER_PIPE_HPP
#define PROCESS_WRAPPER_PIPE_HPP

#include <cstring>
#include "descriptor.hpp"

using Pipe = std::pair<Descriptor, Descriptor>;

Pipe make_pipe();


#endif //PROCESS_WRAPPER_PIPE_HPP
