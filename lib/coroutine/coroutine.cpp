#include <coroutine.h>
#include <iostream>
#include <memory>
#include <queue>
#include <sys/mman.h>
#include <ucontext.h>

namespace Coroutine {

struct Routine;

namespace {

thread_local struct Ordinator {
  static constexpr size_t STACK_SIZE = 16 * 1024;

  std::vector<Routine> routines;
  std::queue<routine_t> finished;
  routine_t current = 0;
  ucontext_t ctx{};
} ordinator;

void entry();

} // namespace

struct Routine {
  RoutineFunction func;
  std::unique_ptr<uint8_t[]> stack;
  bool finished = false;
  ucontext_t ctx;
  std::exception_ptr exception;

  void reset(const RoutineFunction &f) {
    func = f;
    finished = false;
    exception = {};
    ctx.uc_stack.ss_sp = stack.get();
    ctx.uc_stack.ss_size = Ordinator::STACK_SIZE;
    ctx.uc_link = &ordinator.ctx;
    getcontext(&ctx);
    makecontext(&ctx, entry, 0);
  }

  Routine(const RoutineFunction &f)
      : func(f), stack(std::make_unique<uint8_t[]>(Ordinator::STACK_SIZE)) {

    getcontext(&ctx);
    ctx.uc_stack.ss_sp = stack.get();
    ctx.uc_stack.ss_size = Ordinator::STACK_SIZE;
    ctx.uc_link = &ordinator.ctx;
    makecontext(&ctx, entry, 0);
  }

  Routine(const Routine &) = delete;
  Routine(Routine &&) noexcept = default;
  Routine &operator=(const Routine &) = delete;
  Routine &operator=(Routine &&) noexcept = default;
};

routine_t create(const RoutineFunction &function) {
  auto &o = ordinator;
  if (o.finished.empty()) {
    o.routines.emplace_back(function);
    return o.routines.size();
  } else {
    routine_t id = o.finished.front();
    o.finished.pop();
    auto &routine = o.routines[id - 1];
    routine.reset(function);
    return id;
  }
}

bool resume(routine_t id) {
  auto &o = ordinator;
  if (id == 0 || id > o.routines.size()) {
    return false;
  }

  if (o.routines[id - 1].finished) {
    return false;
  }

  o.current = id;

  if (swapcontext(&o.ctx, &o.routines[id - 1].ctx) < 0) {
    o.current = 0;
    return false;
  }
  if (o.routines[id - 1].exception)
    std::rethrow_exception(o.routines[id - 1].exception);

  return !o.routines[id - 1].finished;
}

void yield() {
  auto &o = ordinator;
  routine_t id = o.current;
  o.current = 0;
  swapcontext(&o.routines[id - 1].ctx, &o.ctx);
}

routine_t current() { return ordinator.current; }

namespace {

void entry() {
  auto &o = ordinator;
  routine_t id = o.current;

  if (o.routines[id - 1].func)
    try {
      o.routines[id - 1].func();
    } catch (...) {
      o.routines[id - 1].exception = std::current_exception();
    }
  o.routines[id - 1].finished = true;
  o.current = 0;
  o.finished.emplace(id);
}

} // namespace

} // namespace Coroutine
