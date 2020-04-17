#ifndef PROCESS_WRAPPER_INCLUDE_SHMEM_SHMUTEX_HPP_
#define PROCESS_WRAPPER_INCLUDE_SHMEM_SHMUTEX_HPP_

#include "shared_region.hpp"
#include <semaphore.h>
#include <stdexcept>

namespace shmem {
// удовлетворяет BasicLockable -
// https://en.cppreference.com/w/cpp/named_req/BasicLockable
class ShMutex {
public:
  explicit ShMutex(SharedRegionView &region);

  void lock();

  void unlock();

  ~ShMutex() noexcept;

private:
  sem_t *sem_{};
};
} // namespace shmem
#endif // PROCESS_WRAPPER_INCLUDE_SHMEM_SHMUTEX_HPP_
