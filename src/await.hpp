#pragma once
#include "observable.hpp"
#include "coroutine.hpp"

// await: creates a callable that resumes the current coroutine/thread, subscribe to
// the future and suspends the current coroutine/thread

template <typename T> T await(observable<T> &obs) {
  T value_tmp;
  // std::clog << "await enter\n";
  auto ts = coroutine::current();
  if (!ts) {
    std::clog << "ABORT: await() called outside a resumable task\n";
    std::terminate();
  }

  subscription sub;
  obs.subscribe(sub, [&value_tmp, ts](const T &value) {
    value_tmp = value;
    // std::clog << "await callback\n";
    ts->resume();
  });

  // what if the observable triggers here?
  // in this case, the task is not yet suspended
  // and will resume here, causing an abort (in task_state::resume)

  coroutine::suspend();
  return value_tmp;
}

inline void await(observable<void> &obs) {
  auto ts = coroutine::current();
  if (!ts) {
    std::clog << "ABORT: await() called outside a resumable task\n";
    std::terminate();
  }

  subscription sub;
  obs.subscribe(sub, [ts]() { ts->resume(); });
  coroutine::suspend();
}
