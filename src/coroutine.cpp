#include "coroutine.hpp"

thread_local coroutine *coroutine::current_task = nullptr;
