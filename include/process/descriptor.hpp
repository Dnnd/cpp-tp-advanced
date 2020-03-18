#ifndef PROCESS_WRAPPER_DESCRIPTOR_HPP
#define PROCESS_WRAPPER_DESCRIPTOR_HPP

#include <cstddef>

class Descriptor {
private:
  static constexpr int FD_CLOSED = -1;

public:
  explicit Descriptor(int fd);
  Descriptor();

  Descriptor(Descriptor &&other) noexcept;

  Descriptor(const Descriptor &other) = delete;

  Descriptor &operator=(const Descriptor &other) = delete;

  Descriptor &operator=(Descriptor &&other) noexcept;

  explicit operator int() const;

  void bind(int fd);

  void close() noexcept;

  ~Descriptor();

  bool isClosed() const;

private:
  int fd_;
};

#endif // PROCESS_WRAPPER_DESCRIPTOR_HPP
