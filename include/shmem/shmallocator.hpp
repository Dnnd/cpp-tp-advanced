#ifndef PROCESS_WRAPPER_INCLUDE_SHMEM_SHMALLOCATOR_HPP_
#define PROCESS_WRAPPER_INCLUDE_SHMEM_SHMALLOCATOR_HPP_

#include "shmutex.hpp"
#include <mutex>
namespace shmem {
template <typename T> class ShmAllocator {
public:
  using value_type = T;
  using difference_type =
      typename std::pointer_traits<value_type *>::difference_type;
  using size_type = std::make_unsigned_t<difference_type>;

  explicit ShmAllocator(SharedRegionView *region)
      : state{region}, mutex_{*region} {}

  template <typename U> ShmAllocator(const ShmAllocator<U> &other);

  ShmAllocator(const ShmAllocator &other) noexcept = default;
  ShmAllocator(ShmAllocator &&other) noexcept = default;

  value_type *allocate(size_type n);
  void deallocate(value_type *p, size_type n);
  using propagate_on_container_copy_assignment = std::true_type;
  ShmAllocator select_on_container_copy_construction();

  template <typename U> friend class ShmAllocator;

private:
  T *state_begin();
  T *state_end();
  ShMutex mutex_;
  SharedRegionView *state;
};

template <typename T>
template <typename U>
ShmAllocator<T>::ShmAllocator(const ShmAllocator<U> &other)
    : state{other.state}, mutex_{other.mutex_} {}

template <typename T>
typename ShmAllocator<T>::value_type *
ShmAllocator<T>::allocate(ShmAllocator::size_type n) {
  std::lock_guard<ShMutex> lock{mutex_};
  if (state_begin() - state_end() < n) {
    throw std::bad_alloc();
  }
  char *mem_ptr = state->begin;
  state->begin += n * sizeof(value_type);
  return reinterpret_cast<value_type *>(mem_ptr);
}

template <typename T>
void ShmAllocator<T>::deallocate(value_type *p, ShmAllocator::size_type n) {
  std::lock_guard<ShMutex> lock{mutex_};
  if (state_end() - n == p) {
    state->begin -= n * sizeof(value_type);
  }
}

template <typename T>
ShmAllocator<T> ShmAllocator<T>::select_on_container_copy_construction() {
  return *this;
}

template <typename T> T *ShmAllocator<T>::state_begin() {
  return reinterpret_cast<T *>(state->begin);
}

template <typename T> T *ShmAllocator<T>::state_end() {
  return reinterpret_cast<T *>(state->end);
}
} // namespace shmem
#endif // PROCESS_WRAPPER_INCLUDE_SHMEM_SHMALLOCATOR_HPP_
