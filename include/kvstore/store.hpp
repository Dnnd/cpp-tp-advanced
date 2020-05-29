#ifndef PROCESS_WRAPPER_INCLUDE_KVSTORE_STORE_HPP_
#define PROCESS_WRAPPER_INCLUDE_KVSTORE_STORE_HPP_

#include "kvstore/exceptions.hpp"
#include "kvstore/file.hpp"
#include "kvstore/index.hpp"
#include <filesystem>

template <typename Key, typename Data> class KVStore {
public:
  KVStore(std::filesystem::path path, std::size_t size,
          std::size_t keys_per_index_page)
      : file_{path, size, File::Mode::READONLY} {
    if (size % sizeof(Data) != 0) {
      throw Exception("size is not multiply of sizeof(Data)");
    }
    index_ = KVIndex<Key, Data>{file_, keys_per_index_page};
  }

  std::optional<Data> get(Key key) {
    auto page_bounds = index_.find_page(key);
    if (page_bounds) {
      auto &&[page_begin, page_end] = page_bounds.value();
      return find_by_key(key, page_begin, page_end);
    }
    return {};
  }

private:
  std::optional<Data> find_by_key(Key key, size_t offset_begin,
                                  size_t offset_end) {
    const size_t data_len = sizeof(Data);
    size_t end = offset_end / data_len;
    size_t begin = offset_begin / data_len;
    while (begin <= end) {
      size_t mid = begin + (end - begin) / 2;
      Data middata = file_.read_at<Data>(+mid * data_len);
      if (middata.key == key) {
        return middata;
      } else if (key < middata.key) {
        end = mid - 1;
      } else {
        begin = mid + 1;
      }
    }
    return {};
  }

private:
  File file_;
  KVIndex<Key, Data> index_;
};
#endif // PROCESS_WRAPPER_INCLUDE_KVSTORE_STORE_HPP_
