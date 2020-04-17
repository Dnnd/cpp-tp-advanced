#include <cstring>
#include <iostream>
#include <map>
#include <mutex>
#include <semaphore.h>
#include <sys/mman.h>
#include <unistd.h>
#include <wait.h>

#include "shmem/shmallocator.hpp"
#include "shmem/shmap.hpp"

int main() {
  using SharedString = std::basic_string<char, std::char_traits<char>,
                                         shmem::ShmAllocator<char>>;
  constexpr size_t mem_pool_size = 1024 * 1024 * 10; // 10 KiB
  shmem::SharedRegion region{mem_pool_size};
  shmem::SharedRegionView *view = region.view();

  shmem::ShMap<SharedString, int> map{view};
  auto allocator = map.get_allocator();
  // Используется один и тот же регион памяти для всех shared-объектов
  SharedString key_1 = {"long_long_long_string_1", allocator};
  SharedString key_2 = {"long_long_long_string_2", allocator};
  SharedString key_3 = {"long_long_long_string_3", allocator};
  SharedString key_4 = {"long_long_long_string_4", allocator};

  std::cout.flush();
  pid_t pid = fork();
  switch (pid) {
  case -1:
    return EXIT_FAILURE;
  case 0: {
    // child обавляет нечетные ключи и может не увидеть четные
    auto [key_1_it, key_1_ok] = map.insert({key_1, 1});
    auto [key_3_it, key_3_ok] = map.insert({key_3, 3});
    for (auto &i : {key_1, key_2, key_3, key_4}) {
      auto it = map.find(i);
      if (it == map.end()) {
        std::cout << "[child]: no " << i << '\n';
      } else {
        std::cout << "[child]: " << it->first << " : " << it->second << '\n';
      }
    }
    // child не должен вызывать деструктор shmap
    exit(EXIT_SUCCESS);
  }
  default:
    // parent добавляет четные ключи и может не увидеть нечетные
    auto [key_2_it, key_2_ok] = map.insert({key_2, 2});
    auto [key_4_it, key_4_ok] = map.insert({key_4, 4});
    for (auto &i : {key_1, key_2, key_3, key_4}) {
      auto it = map.find(i);
      if (it == map.end()) {
        std::cout << "[parent]: no " << i << '\n';
      } else {
        std::cout << "[parent]: " << it->first << " : " << it->second << '\n';
      }
    }
  }
  wait(nullptr);
}