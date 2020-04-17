#ifndef PROCESS_WRAPPER_INCLUDE_TCP_SHMAP_HPP_
#define PROCESS_WRAPPER_INCLUDE_TCP_SHMAP_HPP_

#include "shmem/shmallocator.hpp"
#include <map>
#include <utility>

namespace shmem {
template <typename Key, typename Value, typename Comparator = std::less<Key>>
class ShMap {
public:
  using allocator_type = ShmAllocator<std::pair<const Key, Value>>;
  using container_type = std::map<Key, Value, Comparator, allocator_type>;

  explicit ShMap(SharedRegionView *region);

  auto insert(const std::pair<const Key, Value> &pair);

  auto insert(std::pair<const Key, Value> &&pair);

  auto insert_or_assign(const std::pair<const Key, Value> &pair);

  auto insert_or_assign(std::pair<const Key, Value> &&pair);

  auto erase(Key &&key);

  auto erase(const Key &key);

  auto find(const Key &key) const;

  auto find(Key &&key) const;

  allocator_type get_allocator() const noexcept;

  auto end() const;

  ~ShMap();

private:
  mutable ShMutex mutex_;
  container_type *container_;
};

template <typename Key, typename Value, typename Comparator>
ShMap<Key, Value, Comparator>::ShMap(SharedRegionView *region)
    : mutex_{*region}, container_{} {
  auto allocator = allocator_type{region};
  container_ = shmnew<container_type>(*region, allocator);
}

template <typename Key, typename Value, typename Comparator>
auto ShMap<Key, Value, Comparator>::insert(
    const std::pair<const Key, Value> &pair) {
  std::lock_guard<ShMutex> lock{mutex_};
  return container_->insert(pair);
}

template <typename Key, typename Value, typename Comparator>
auto ShMap<Key, Value, Comparator>::insert(std::pair<const Key, Value> &&pair) {
  std::lock_guard<ShMutex> lock{mutex_};
  return container_->insert(std::move(pair));
}

template <typename Key, typename Value, typename Comparator>
auto ShMap<Key, Value, Comparator>::insert_or_assign(
    const std::pair<const Key, Value> &pair) {
  std::lock_guard<ShMutex> lock{mutex_};
  return container_->insert_or_assign(pair);
}

template <typename Key, typename Value, typename Comparator>
auto ShMap<Key, Value, Comparator>::insert_or_assign(
    std::pair<const Key, Value> &&pair) {
  std::lock_guard<ShMutex> lock{mutex_};
  return container_->insert_or_assign(std::move(pair));
}

template <typename Key, typename Value, typename Comparator>
auto ShMap<Key, Value, Comparator>::erase(Key &&key) {
  std::lock_guard<ShMutex> lock{mutex_};
  return container_->erase(std::move(key));
}

template <typename Key, typename Value, typename Comparator>
auto ShMap<Key, Value, Comparator>::erase(const Key &key) {
  std::lock_guard<ShMutex> lock{mutex_};
  return container_->erase(key);
}

template <typename Key, typename Value, typename Comparator>
auto ShMap<Key, Value, Comparator>::find(const Key &key) const {
  std::lock_guard<ShMutex> lock{mutex_};
  return container_->find(key);
}

template <typename Key, typename Value, typename Comparator>
auto ShMap<Key, Value, Comparator>::find(Key &&key) const {
  std::lock_guard<ShMutex> lock{mutex_};
  return container_->find(std::move(key));
}

template <typename Key, typename Value, typename Comparator>
typename ShMap<Key, Value, Comparator>::allocator_type
ShMap<Key, Value, Comparator>::get_allocator() const noexcept {
  return container_->get_allocator();
}

template <typename Key, typename Value, typename Comparator>
ShMap<Key, Value, Comparator>::~ShMap() {
  container_->~container_type();
}

template <typename Key, typename Value, typename Comparator>
auto ShMap<Key, Value, Comparator>::end() const {
  return container_->end();
}
} // namespace shmem
#endif // PROCESS_WRAPPER_INCLUDE_TCP_SHMAP_HPP_
