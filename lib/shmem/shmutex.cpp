#include "shmem/shmutex.hpp"
#include "shmem/exceptions.hpp"
namespace shmem {

void ShMutex::lock() { sem_.lock(); }
void ShMutex::unlock() { sem_.unlock(); }
ShMutex::ShMutex(ShmAllocator<sem_t> allocator)
    : sem_{}, allocator_{allocator} {
  sem_t *sem = allocator.allocate(1);
  allocator.construct(sem); // для default-инициализации

  if (-1 == sem_init(sem, 1, 1)) {
    throw MutexException("unable to initialize semaphore", errno);
  }
  sem_ = LockableSem{sem};
}
ShMutex &ShMutex::operator=(ShMutex &&other) noexcept {
  ShMutex tmp{std::move(other)};
  swap(tmp);
  return *this;
}
ShMutex::~ShMutex() {
  if (sem_.isEmpty()) {
    return;
  }
  sem_t *unwrapped = sem_.unwrap();
  sem_destroy(unwrapped);
  // allocator_.destroy можно не вызывать, т.к. деструктор sem_t - тривиальный
  allocator_.deallocate(unwrapped, 1);
}
ShMutex::ShMutex(ShMutex &&other) noexcept
    : sem_{other.sem_.unwrap()}, allocator_{other.allocator_} {}

void ShMutex::swap(ShMutex &other) {
  std::swap(sem_, other.sem_);
  std::swap(allocator_, other.allocator_);
}
} // namespace shmem