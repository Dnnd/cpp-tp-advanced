#ifndef PROCESS_WRAPPER_INCLUDE_KVSTORE_FILE_HPP_
#define PROCESS_WRAPPER_INCLUDE_KVSTORE_FILE_HPP_

#include <cstdint>
#include <cstring>
#include <filesystem>

class File {
public:
  enum class Mode : uint32_t { READONLY = 0, READWRITE = 1 };
  File(const std::filesystem::path &path, std::size_t size, Mode mode);
  File(File &&other) noexcept ;
  File& operator=(File &&other) noexcept ;
  File(const File &other) = delete;
  File& operator=(const File &other) = delete ;

  template <typename T> void write_at(const T &obj, size_t position) noexcept {
    static_assert(std::is_trivially_copyable_v<T>, "T is not pod");
    std::memcpy(data_ + position, &obj, sizeof(T));
  }
  template <typename T> T read_at(size_t position) const noexcept {
    static_assert(std::is_trivially_copyable_v<T>, "T is not pod");
    T t;
    std::memcpy(&t, data_ + position, sizeof(T));
    return t;
  }

  [[nodiscard]] std::size_t size() const noexcept;

  ~File() noexcept;

private:
  char *data_;
  std::size_t size_;
};
#endif // PROCESS_WRAPPER_INCLUDE_KVSTORE_FILE_HPP_
