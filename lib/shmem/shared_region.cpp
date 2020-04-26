#include "shmem/shared_region.hpp"
#include <new>
#include <semaphore.h>
#include <shmem/exceptions.hpp>
#include <sys/mman.h>

namespace shmem {

SharedRegion::SharedRegion(size_t n) : size_{n}, mem_{nullptr} {
  void *region_begin = mmap(nullptr, n, PROT_READ | PROT_WRITE,
                            MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  if (region_begin == MAP_FAILED) {
    throw std::bad_alloc();
  }
  // Конструируем семафор в начале выделенной памяти
  // ShMutex использовать нельзя, из-за цепочки зависимостей
  // ShMutex <- ShmAllocator <- SharedRegionView
  // SharedRegionView еще не сконструирован

  // Конструируем SharedRegionView в начале SharedRegion, после семафора

  auto *free_mem_begin = static_cast<char *>(region_begin) +
                         sizeof(SharedRegionView) + sizeof(sem_t);
  auto *sem_mem = reinterpret_cast<sem_t *>(static_cast<char *>(region_begin) +
                                            sizeof(SharedRegionView));

  //конструкторы тривиальные, не выбросят исключений
  new (sem_mem) sem_t{};

  new (region_begin) SharedRegionView{
      .begin = free_mem_begin,
      .end = free_mem_begin + size_,
      .semaphore = sem_mem,
  };

  if (-1 == sem_init(sem_mem, 1, 1)) {
    int old_errno = errno;
    // игнорируем потенциальные ошибки munmap
    munmap(region_begin, size_);
    throw ErrnoException("unable to initialize semaphore in shared region",
                         old_errno);
  }

  mem_ = region_begin;
}

SharedRegionView *SharedRegion::view() {
  return reinterpret_cast<SharedRegionView *>(mem_);
}

SharedRegion::~SharedRegion() {
  sem_destroy(view()->semaphore);
  munmap(mem_, size_);
}

} // namespace shmem
