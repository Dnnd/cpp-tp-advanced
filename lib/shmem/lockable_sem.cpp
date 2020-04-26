#include "shmem/lockable_sem.hpp"
#include "shmem/exceptions.hpp"
#include <cerrno>
namespace shmem {

void LockableSem::lock() {
  if (-1 == sem_wait(sem_)) {
    throw MutexException("unable to lock", errno);
  };
}

void LockableSem::unlock() {
  if (-1 == sem_post(sem_)) {
    throw MutexException("unable to unlock", errno);
  };
}
LockableSem::LockableSem(sem_t *sem) noexcept : sem_{sem} {}
sem_t *LockableSem::unwrap() noexcept {
  sem_t *tmp = sem_;
  sem_ = nullptr;
  return tmp;
}
LockableSem::LockableSem() noexcept : sem_{nullptr} {}
bool LockableSem::isEmpty() const noexcept { return sem_ == nullptr; }

} // namespace shmem