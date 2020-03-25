#include "process/process.hpp"
#include <cstring>
#include <filesystem>
#include <process/pipe.hpp>
#include <stdexcept>
#include <wait.h>
using namespace std::string_literals;

Process::Process(const std::string &path, const std::vector<std::string> &args)
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

    child_read.close();
    child_write.close();

    char *filename = const_cast<char *>(std::filesystem::path(path).c_str());
    std::vector<char *> syscall_args{filename};
    syscall_args.reserve(args.size() + 1);
    std::transform(
        args.begin(), args.end(), std::back_inserter(syscall_args),
        [](const std::string &arg) { return const_cast<char *>(arg.data()); });
    syscall_args.push_back(NULL);

    if (execv(path.c_str(), syscall_args.data()) == -1) {
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

Process::~Process() noexcept {
  try {
    close();
  } catch (...) {
    // явно игнорируем исключения в деструкторе
  }
  // ждем в цикле на случай, если waitpid прервали сигналом (errno & EINTR)
  while (true) {
    pid_t ret = waitpid(child_pid_, NULL, 0);
    if (ret == child_pid_ || (ret == -1 && !(errno & EINTR))) {
      return;
    }
  }
}

std::size_t Process::read(void *data, std::size_t len) {
  ssize_t got = ::read(reader_.getUnderlyingDescriptor(), data, len);
  if (got == -1) {
    throw std::runtime_error("unable to perform Process::read "s +
                             std::strerror(errno));
  }
  return got;
}

std::size_t Process::write(const void *data, std::size_t len) {
  ssize_t sent = ::write(writer_.getUnderlyingDescriptor(), data, len);
  if (sent == -1) {
    throw std::runtime_error(
        "fail to perform write() syscall in Process::write "s +
        std::strerror(errno));
  }
  return sent;
}

void Process::readExact(void *data, std::size_t len) {
  std::size_t total = 0;
  while (total < len) {
    std::size_t got = read(static_cast<char *>(data) + total, len - total);
    if (total != len && got == 0) {
      throw std::runtime_error("EOF encountered in readExact method, read "s +
                               std::to_string(total) + " bytes");
    }
    total += got;
  }
}

void Process::writeExact(const void *data, std::size_t len) {
  std::size_t total = 0;
  while (total < len) {
    std::size_t sent =
        write(static_cast<const char *>(data) + total, len - total);
    if (total != len && sent == 0) {
      throw std::runtime_error("EOF encountered in writeExact method, wrote "s +
                               std::to_string(total) + " bytes");
    }
    total += sent;
  }
}

void Process::closeStdin() { writer_.close(); }

void Process::close() {
  closeStdin();
  reader_.close();
}
bool Process::isReadable() const { return !reader_.isClosed(); }
