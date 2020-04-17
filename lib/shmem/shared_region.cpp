#include "shmem/shared_region.hpp"
#include <new>
#include <sys/mman.h>

namespace shmem {
SharedRegion::~SharedRegion() noexcept { munmap(mem_, size_); }
SharedRegion::SharedRegion(size_t n) : size_{n}, mem_{nullptr} {
  void *mem = mmap(nullptr, n, PROT_READ | PROT_WRITE,
                   MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  if (mem == MAP_FAILED) {
    throw std::bad_alloc();
  }
  // Конструируем SharedRegionView в начале SharedRegion
  auto *view = new (mem) SharedRegionView{};
  view->begin = static_cast<char *>(mem) + sizeof(SharedRegionView);
  view->end = view->begin + n;
  mem_ = mem;
}
SharedRegionView *SharedRegion::view() {
  return reinterpret_cast<SharedRegionView *>(mem_);
}
} // namespace shmem
