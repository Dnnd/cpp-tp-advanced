#ifndef PROCESS_WRAPPER_INCLUDE_SHMEM_SHARED_REGION_HPP_
#define PROCESS_WRAPPER_INCLUDE_SHMEM_SHARED_REGION_HPP_

#include <cstddef>
#include <new>
namespace shmem {
struct SharedRegionView {
  char *begin{nullptr};
  char *end{nullptr};
};

class SharedRegion {
public:
  explicit SharedRegion(size_t n);

  ~SharedRegion() noexcept;
  SharedRegionView *view();

private:
  size_t size_;
  void *mem_;
};

template <typename T, typename... Args>
T *shmnew(SharedRegionView &region, Args... args) {
  if (region.end - region.begin < sizeof(T)) {
    throw std::bad_alloc();
  }

  auto mem_ptr = reinterpret_cast<T *>(region.begin);
  region.begin += sizeof(T);
  return new (mem_ptr) T{std::forward<Args>(args)...};
}
} // namespace shmem
#endif // PROCESS_WRAPPER_INCLUDE_SHMEM_SHARED_REGION_HPP_
