#ifndef PROCESS_WRAPPER_INCLUDE_KVSTORE_DESCRIPTOR_HPP_
#define PROCESS_WRAPPER_INCLUDE_KVSTORE_DESCRIPTOR_HPP_
class Descriptor {
public:
  explicit Descriptor(int fd);

  [[nodiscard]] int get() const noexcept;

  ~Descriptor() noexcept;

private:
  int fd_;
};
#endif // PROCESS_WRAPPER_INCLUDE_KVSTORE_DESCRIPTOR_HPP_
