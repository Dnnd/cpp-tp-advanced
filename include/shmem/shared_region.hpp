#ifndef PROCESS_WRAPPER_INCLUDE_SHMEM_SHARED_REGION_HPP_
#define PROCESS_WRAPPER_INCLUDE_SHMEM_SHARED_REGION_HPP_

#include <cstddef>
#include <semaphore.h>

namespace shmem {
struct SharedRegionView {
  char *begin{nullptr};
  char *end{nullptr};
  sem_t *semaphore{nullptr};
};

class SharedRegion {
public:
  explicit SharedRegion(size_t n);

  SharedRegionView *view();

  ~SharedRegion();

private:
  size_t size_;
  void *mem_;
};

} // namespace shmem
#endif // PROCESS_WRAPPER_INCLUDE_SHMEM_SHARED_REGION_HPP_
