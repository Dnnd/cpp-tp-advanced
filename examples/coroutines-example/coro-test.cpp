#include "coroutine.h"

void routine(int min, int max) {
  std::vector<int> k;
  for (auto i = min; i < max; ++i) {
    k.push_back(i);
    Coroutine::yield();
  }
}

int main() {
  auto coro_1 = Coroutine::create(routine, 0, 10);
  Coroutine::resume(coro_1);
  for (auto i = 0; i < 100; ++i) {
    Coroutine::create_and_run(routine, 0, 10);
  }
  while (!Coroutine::resume(coro_1)) {
  };
}