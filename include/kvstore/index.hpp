#ifndef PROCESS_WRAPPER_INCLUDE_KVSTORE_INDEX_HPP_
#define PROCESS_WRAPPER_INCLUDE_KVSTORE_INDEX_HPP_
#include "kvstore/file.hpp"
#include <map>
#include <utility>

template <typename Key, typename Data> class KVIndex {
public:
  KVIndex() = default;
  using PageBounds = std::pair<std::size_t, std::size_t>;
  KVIndex(const File &file, size_t keys_per_index_page) {
    std::size_t data_size = sizeof(Data);
    page_size = data_size * keys_per_index_page;
    last_page_size = file.size() % page_size;
    last_page_size = (last_page_size == 0) ? page_size : last_page_size;
    for (size_t i = 0; i < file.size(); i += page_size) {
      Data d = file.read_at<Data>(i);
      store_[d.key] = i;
    }
  }

  std::optional<PageBounds> find_page(Key key) {
    auto itlow = store_.lower_bound(key);
    if (itlow->first == key) {
      return {{itlow->second, itlow->second + page_size}};
    } else if (itlow != store_.begin() && itlow != store_.end()) {
      itlow--;
      return {{itlow->second, itlow->second + page_size}};
    } else if (itlow == store_.end()) {
      itlow--;
      return {{itlow->second, itlow->second + last_page_size}};
    }
    return {};
  }

private:
  std::map<Key, std::size_t> store_;
  size_t page_size{0};
  size_t last_page_size{0};
};
#endif // PROCESS_WRAPPER_INCLUDE_KVSTORE_INDEX_HPP_
