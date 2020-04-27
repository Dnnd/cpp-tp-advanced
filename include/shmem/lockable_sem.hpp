#ifndef PROCESS_WRAPPER_INCLUDE_SHMEM_LOCKABLE_SEM_HPP_
#define PROCESS_WRAPPER_INCLUDE_SHMEM_LOCKABLE_SEM_HPP_

#include <semaphore.h>
namespace shmem {

// невладеющая обертка над sem_t
// удовлетворяет BasicLockable -
// https://en.cppreference.com/w/cpp/named_req/BasicLockable
class LockableSem {
public:
  // требует, чтобы sem уже был проинициализирован
  explicit LockableSem(sem_t *sem) noexcept;
  LockableSem() noexcept;
  void lock();
  void unlock();
  [[nodiscard]] bool isEmpty() const noexcept;
  [[nodiscard]] sem_t *unwrap() noexcept;

private:
  sem_t *sem_;
};
} // namespace shmem
#endif // PROCESS_WRAPPER_INCLUDE_SHMEM_LOCKABLE_SEM_HPP_
