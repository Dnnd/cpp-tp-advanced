#ifndef COROUTINE_H
#define COROUTINE_H

#include <functional>

namespace Coroutine
{

using routine_t = unsigned long;
using RoutineFunction = std::function<void()>;

routine_t create(const RoutineFunction& function);
bool resume(routine_t id);
void yield();
routine_t current();

template <class F, class ...Args, class = std::enable_if_t<!std::is_invocable_v<F>>>
routine_t create(F&& f, Args&&... args)
{
	return create(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
}

template <class F, class ...Args>
bool create_and_run(F&& f, Args&&... args)
{
	return resume(create(std::forward<F>(f), std::forward<Args>(args)...));
}

}

#endif //COROUTINE_H
