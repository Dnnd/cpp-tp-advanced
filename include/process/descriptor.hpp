#ifndef PROCESS_WRAPPER_DESCRIPTOR_HPP
#define PROCESS_WRAPPER_DESCRIPTOR_HPP

#include <cstddef>

class Descriptor {
public:
  explicit Descriptor(int fd);
  Descriptor();

  Descriptor(Descriptor &&other) noexcept;

  Descriptor(const Descriptor &other) = delete;

  Descriptor &operator=(const Descriptor &other) = delete;

  Descriptor &operator=(Descriptor &&other) noexcept;

  int getUnderlyingDescriptor() const noexcept;

  void bind(int fd);

  void close();

  ~Descriptor() noexcept ;

  bool isClosed() const;

private:
  int fd_;
};

#endif // PROCESS_WRAPPER_DESCRIPTOR_HPP
