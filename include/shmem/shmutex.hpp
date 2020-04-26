#ifndef PROCESS_WRAPPER_INCLUDE_SHMEM_SHMUTEX_HPP_
#define PROCESS_WRAPPER_INCLUDE_SHMEM_SHMUTEX_HPP_

#include "lockable_sem.hpp"
#include "shared_region.hpp"
#include "shmem/shmallocator.hpp"
#include <semaphore.h>
#include <stdexcept>

namespace shmem {

// владеющая обертка над sem_t
// cамостоятельно размещает sem_t в shared-memory через allocator
// удовлетворяет BasicLockable -
// https://en.cppreference.com/w/cpp/named_req/BasicLockable
class ShMutex {
public:
  explicit ShMutex(ShmAllocator<sem_t> allocator);

  ShMutex(ShMutex &&other) noexcept;
  ShMutex &operator=(ShMutex &&other) noexcept;

  ShMutex(const ShMutex &other) = delete;
  ShMutex &operator=(const ShMutex &other) = delete;

  void swap(ShMutex &other) noexcept;

  void lock();

  void unlock();

  ~ShMutex();

private:
  LockableSem sem_;
  ShmAllocator<sem_t> allocator_;
};
} // namespace shmem
#endif // PROCESS_WRAPPER_INCLUDE_SHMEM_SHMUTEX_HPP_
