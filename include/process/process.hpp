#ifndef PROCESS_WRAPPER_PROCESS_HPP
#define PROCESS_WRAPPER_PROCESS_HPP

#include <string>
#include <cstddef>
#include <unistd.h>
#include "process/descriptor.hpp"

class Process {
public:
    explicit Process(const std::string &path);

    ~Process();

    std::size_t read(void *data, size_t len);

    std::size_t write(const void *data, size_t len);

    void readExact(void *data, size_t len);

    void writeExact(const void *data, size_t len);

    void closeStdin();

private:
    pid_t child_pid_;
    Descriptor reader_;
    Descriptor writer_;

    void close();
};

#endif //PROCESS_WRAPPER_PROCESS_HPP
