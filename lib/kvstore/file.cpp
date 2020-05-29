#include "kvstore/file.hpp"
#include <cstring>
#include <kvstore/descriptor.hpp>
#include <sys/fcntl.h>
#include <sys/mman.h>

File::File(const std::filesystem::path &path, std::size_t size, File::Mode mode)
    : size_{size} {
  if (mode == File::Mode::READONLY) {
    Descriptor fd{open(path.c_str(), O_RDONLY)};
    char *file = static_cast<char *>(
        mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd.get(), 0));
    if (file == MAP_FAILED) {
      throw std::runtime_error(std::strerror(errno));
    }
    data_ = file;
    return;
  }

  Descriptor fd{open(path.c_str(), O_RDWR)};
  char *file = static_cast<char *>(
      mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd.get(), 0));
  if (file == MAP_FAILED) {
    throw std::runtime_error(std::strerror(errno));
  }
  data_ = file;
}

File::~File() noexcept {
  if (data_) {
    munmap(data_, size_);
  }
}

std::size_t File::size() const noexcept { return size_; }
File::File(File &&other) noexcept : data_{other.data_}, size_{other.size_} {
  other.data_ = nullptr;
  other.size_ = 0;
}
File &File::operator=(File &&other) noexcept {
  File tmp{std::move(other)};
  std::swap(data_, tmp.data_);
  std::swap(size_, tmp.size_);
  return *this;
}
