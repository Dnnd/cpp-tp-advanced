#include "shmem/shmutex.hpp"
namespace shmem {
ShMutex::ShMutex(SharedRegionView &region) {
  sem_t *sem = shmnew<sem_t>(region);
  if (-1 == sem_init(sem, 1, 1)) {
    throw std::runtime_error("unableto construct shmutex");
  };
  sem_ = sem;
}
void ShMutex::lock() {
  if (-1 == sem_wait(sem_)) {
    throw std::runtime_error("unable to lock");
  };
}
void ShMutex::unlock() {
  if (-1 == sem_post(sem_)) {
    throw std::runtime_error("unable to unlock");
  };
}
ShMutex::~ShMutex() noexcept { sem_destroy(sem_); }
} // namespace shmem