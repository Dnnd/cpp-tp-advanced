#ifndef PROCESS_WRAPPER_PROCESS_HPP
#define PROCESS_WRAPPER_PROCESS_HPP

#include "process/descriptor.hpp"
#include <cstddef>
#include <string>
#include <unistd.h>

class Process {
public:
  explicit Process(const std::string &path);

  ~Process();

  std::size_t read(void *data, std::size_t len);

  std::size_t write(const void *data, std::size_t len);

  void readExact(void *data, std::size_t len);

  void writeExact(const void *data, std::size_t len);

  void closeStdin();

  void close();

private:
  pid_t child_pid_;
  Descriptor reader_;
  Descriptor writer_;
};

#endif // PROCESS_WRAPPER_PROCESS_HPP
