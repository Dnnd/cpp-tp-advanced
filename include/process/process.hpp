#ifndef PROCESS_WRAPPER_PROCESS_HPP
#define PROCESS_WRAPPER_PROCESS_HPP

#include "process/descriptor.hpp"
#include <cstddef>
#include <string>
#include <unistd.h>
#include <vector>

class Process {
public:
  // Помечен как explicit, потому что у args есть значение по-умолчанию
  explicit Process(const std::string &path,
                   const std::vector<std::string> &args = {});

  ~Process() noexcept;

  std::size_t read(void *data, std::size_t len);

  std::size_t write(const void *data, std::size_t len);

  void readExact(void *data, std::size_t len);

  void writeExact(const void *data, std::size_t len);

  bool isReadable() const;

  void closeStdin();

  void close();

private:
  pid_t child_pid_;
  Descriptor reader_;
  Descriptor writer_;
};

#endif // PROCESS_WRAPPER_PROCESS_HPP
