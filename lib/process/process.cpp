#include "process/process.hpp"
#include <cstring>
#include <filesystem>
#include <process/pipe.hpp>
#include <stdexcept>
#include <wait.h>
using namespace std::string_literals;

Process::Process(const std::string &path)
    : reader_{}, writer_{}, child_pid_{0} {

  auto [child_read, parent_write] = make_pipe();
  auto [parent_read, child_write] = make_pipe();
  fflush(NULL);
  pid_t child_pid = fork();
  switch (child_pid) {
  case -1: {
    throw std::runtime_error("failed fork() syscall in Process constructor: "s +
                             std::strerror(errno));
  }
  case 0: {
    // Дочерний процесс
    parent_write.close();
    parent_read.close();
    child_read.bind(STDIN_FILENO);
    child_write.bind(STDOUT_FILENO);

    auto filename = std::filesystem::path(path).filename();
    if (execl(path.c_str(), filename.c_str(), NULL) == -1) {
      // Дочерниий процесс не смог выполнить execl, завершаем его
      std::perror(std::strerror(errno));
      exit(EXIT_FAILURE);
    }
    break;
  }
  default: {
    // Родительский процесс
    child_pid_ = child_pid;
    child_read.close();
    child_write.close();
    reader_ = std::move(parent_read);
    writer_ = std::move(parent_write);
    break;
  }
  }
}

Process::~Process() {
  this->close();
  if (waitpid(child_pid_, NULL, WNOHANG) == 0) {
    kill(child_pid_, SIGKILL);
    wait(NULL);
  }
}

std::size_t Process::read(void *data, std::size_t len) {
  return reader_.read(data, len);
}

std::size_t Process::write(const void *data, std::size_t len) {
  return writer_.write(data, len);
}

void Process::readExact(void *data, std::size_t len) {
  reader_.readExact(data, len);
}

void Process::writeExact(const void *data, std::size_t len) {
  writer_.writeExact(data, len);
}

void Process::closeStdin() { writer_.close(); }

void Process::close() {
  writer_.close();
  reader_.close();
}
bool Process::isReadable() const { return !reader_.isClosed(); }
