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

  std::size_t write(const void *data, std::size_t len);

  std::size_t read(void *data, std::size_t len);

  void bind(int fd);

  void writeExact(const void *data, std::size_t len);

  void readExact(void *data, std::size_t len);

  void close() noexcept;

  ~Descriptor();

  bool isClosed() const;

private:
  int fd_;
};

#endif // PROCESS_WRAPPER_DESCRIPTOR_HPP
