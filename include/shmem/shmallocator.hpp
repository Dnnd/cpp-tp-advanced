#ifndef PROCESS_WRAPPER_INCLUDE_SHMEM_SHMALLOCATOR_HPP_
#define PROCESS_WRAPPER_INCLUDE_SHMEM_SHMALLOCATOR_HPP_

#include "shmem/lockable_sem.hpp"
#include "shmem/shared_region.hpp"
#include <mutex>

namespace shmem {
template <typename T> class ShmAllocator {
public:
  using value_type = T;
  using difference_type =
      typename std::pointer_traits<value_type *>::difference_type;
  using size_type = std::make_unsigned_t<difference_type>;

  explicit ShmAllocator(SharedRegionView *region)
      : state{region}, lock_{region->semaphore} {};

  template <typename U>
  ShmAllocator(const ShmAllocator<U> &other)
      : state{other.state}, lock_{other.lock_} {}

  ShmAllocator(const ShmAllocator &other) noexcept = default;
  ShmAllocator(ShmAllocator &&other) noexcept = default;
  ShmAllocator &operator=(const ShmAllocator &other) noexcept = default;
  ShmAllocator &operator=(ShmAllocator &&other) noexcept = default;

  value_type *allocate(size_type n) {
    std::lock_guard lock{lock_};
    if (state_begin() - state_end() < n) {
      throw std::bad_alloc();
    }
    char *mem_ptr = state->begin;
    state->begin += n * sizeof(value_type);
    return reinterpret_cast<value_type *>(mem_ptr);
  }

  void deallocate(value_type *p, size_type n) {
    std::lock_guard lock{lock_};
    if (state_end() - n == p) {
      state->begin -= n * sizeof(value_type);
    }
  }

  template <class... Args> void construct(T *p, Args &&... args) {
    new (p) T{std::forward<Args>(args)...};
  }

  void destroy(T *p) { p->~T(); }

  template <typename U> friend class ShmAllocator;

  template <typename U> bool operator==(const ShmAllocator<U> &other) const {
    return state == other.state;
  }

  template <typename U> bool operator!=(const ShmAllocator<U> &other) const {
    return !(*this == other);
  }

private:
  T *state_begin() { return reinterpret_cast<T *>(state->begin); }
  T *state_end() { return reinterpret_cast<T *>(state->end); }
  SharedRegionView *state;
  LockableSem lock_;
};

} // namespace shmem
#endif // PROCESS_WRAPPER_INCLUDE_SHMEM_SHMALLOCATOR_HPP_
