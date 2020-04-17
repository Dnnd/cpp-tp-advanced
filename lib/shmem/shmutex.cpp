#include "shmem/shmutex.hpp"
#include "shmem/exceptions.hpp"
namespace shmem {
ShMutex::ShMutex(SharedRegionView &region) {
  sem_t *sem = shmnew<sem_t>(region);
  if (-1 == sem_init(sem, 1, 1)) {
    throw MutexException("unable to construct shmutex", errno);
  };
  sem_ = sem;
}
void ShMutex::lock() {
  if (-1 == sem_wait(sem_)) {
    throw MutexException("unable to lock", errno);
  };
}
void ShMutex::unlock() {
  if (-1 == sem_post(sem_)) {
    throw MutexException("unable to unlock", errno);
  };
}
ShMutex::~ShMutex() noexcept { sem_destroy(sem_); }
} // namespace shmem