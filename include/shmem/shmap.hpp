#ifndef PROCESS_WRAPPER_INCLUDE_TCP_SHMAP_HPP_
#define PROCESS_WRAPPER_INCLUDE_TCP_SHMAP_HPP_

#include "shmem/shmallocator.hpp"
#include "shmutex.hpp"
#include <map>
#include <utility>

namespace shmem {

template <typename Key, typename Value, typename Comparator = std::less<Key>>
class ShMap {
public:
  using allocator_type = ShmAllocator<std::pair<const Key, Value>>;
  using container_type = std::map<Key, Value, Comparator, allocator_type>;
  using size_type = typename container_type ::size_type;
  using iterator = typename container_type ::iterator;

  ShMap(const ShMap &other) : ShMap{other.get_allocator()} {
    *container_ = *other.container_;
  }

  ShMap &operator=(const ShMap &other) {
    if (this == &other) {
      return *this;
    }
    ShMap tmp{other};
    swap(tmp);
    return *this;
  }

  ShMap(ShMap &&other) noexcept
      : mutex_(std::move(other.mutex_)), container_{other.container_} {
    other.container_ = nullptr;
  }
  ShMap &operator=(ShMap &&other) noexcept {
    ShMap tmp{std::move(other)};
    swap(tmp);
  }

  explicit ShMap(allocator_type allocator)
      : mutex_{allocator}, container_{nullptr} {
    auto container_allocator = ShmAllocator<container_type>{allocator};
    container_type *container = container_allocator.allocate(1);

    try {
      // Конструктор std::map выбрасывает только исключения от
      // allocator.allocate()
      container_allocator.construct(container, allocator);
    } catch (std::bad_alloc &e) {
      container_allocator.deallocate(container, 1);
    }

    // теперь контейнер сконструирован, для него нужно вызвать деструктор и
    // деаллоцировать память
    container_ = container;
  }

  std::pair<iterator, bool> insert(const std::pair<const Key, Value> &pair) {
    std::lock_guard<ShMutex> lock{mutex_};
    return container_->insert(pair);
  }

  std::pair<iterator, bool> insert(std::pair<const Key, Value> &&pair) {
    std::lock_guard<ShMutex> lock{mutex_};
    return container_->insert(std::move(pair));
  }

  std::pair<iterator, bool>
  insert_or_assign(const std::pair<const Key, Value> &pair) {
    std::lock_guard<ShMutex> lock{mutex_};
    return container_->insert_or_assign(std::move(pair));
  }

  std::pair<iterator, bool>
  insert_or_assign(std::pair<const Key, Value> &&pair) {
    std::lock_guard<ShMutex> lock{mutex_};
    return container_->insert_or_assign(pair);
  }

  size_type erase(Key &&key) {
    std::lock_guard<ShMutex> lock{mutex_};
    return container_->erase(std::move(key));
  }

  size_type erase(const Key &key) {
    std::lock_guard<ShMutex> lock{mutex_};
    return container_->erase(key);
  }

  iterator find(const Key &key) const {
    std::lock_guard<ShMutex> lock{mutex_};
    return container_->find(key);
  }

  iterator find(Key &&key) const {
    std::lock_guard<ShMutex> lock{mutex_};
    return container_->find(std::move(key));
  }

  allocator_type get_allocator() const noexcept {
    return container_->get_allocator();
  }

  iterator end() const { return container_->end(); }

  ~ShMap() {
    ShmAllocator<container_type> container_alloc = container_->get_allocator();
    container_alloc.destroy(container_);
    container_alloc.deallocate(container_, 1);
  }

  void swap(ShMap &other) noexcept {
    mutex_.swap(other.mutex_);
    std::swap(container_, other.container_);
  }

private:
  mutable ShMutex mutex_;
  container_type *container_;
}; // namespace shmem

} // namespace shmem
#endif // PROCESS_WRAPPER_INCLUDE_TCP_SHMAP_HPP_
