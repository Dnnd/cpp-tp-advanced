#ifndef PROCESS_WRAPPER_INCLUDE_SHMEM_SHARED_REGION_HPP_
#define PROCESS_WRAPPER_INCLUDE_SHMEM_SHARED_REGION_HPP_

#include <cstddef>
#include <semaphore.h>

namespace shmem {
struct SharedRegionView {
  // Копирование SharedRegionView любым способом - почти гарантированная ошибка
  // Все клиенты SharedRegionView должны получать SharedRegionView* от
  // SharedRegion Использование копии SharedRegionView приведет к
  // рассинхронизации состояния аллокаторов
  SharedRegionView(const SharedRegionView &other) = delete;
  SharedRegionView(SharedRegionView &&other) = delete;
  SharedRegionView &operator=(const SharedRegionView &other) = delete;
  SharedRegionView &operator=(SharedRegionView &&other) = delete;

  SharedRegionView(char *begin_ptr, char *end_ptr, sem_t *sem_ptr)
      : begin{begin_ptr}, end{end_ptr}, semaphore{sem_ptr} {}

  char *begin{nullptr};
  char *end{nullptr};
  sem_t *semaphore{nullptr};
};

class SharedRegion {
public:
  explicit SharedRegion(size_t n);

  SharedRegion(const SharedRegion &other) = delete;
  SharedRegion(SharedRegion &&other) noexcept;

  SharedRegion& operator=(const SharedRegion& other) = delete;
  SharedRegion& operator=(SharedRegion &&other) noexcept ;

  void swap(SharedRegion &other) noexcept ;

  SharedRegionView *view();

  ~SharedRegion();

private:
  size_t size_;
  void *mem_;
};

} // namespace shmem
#endif // PROCESS_WRAPPER_INCLUDE_SHMEM_SHARED_REGION_HPP_
