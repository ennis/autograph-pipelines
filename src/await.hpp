#pragma once
#include "coroutine.hpp"
#include "observable.hpp"
#include <experimental/resumable>

// await: creates a callable that resumes the current coroutine/thread,
// subscribe to
// the future and suspends the current coroutine/thread

template <typename T> T xxawait(observable<T> &obs) {
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

inline void xxawait(observable<void> &obs) {
  auto ts = coroutine::current();
  if (!ts) {
    std::clog << "ABORT: await() called outside a resumable task\n";
    std::terminate();
  }

  subscription sub;
  obs.subscribe(sub, [ts]() { ts->resume(); });
  coroutine::suspend();
}

// resumable function version
inline auto operator co_await(observable<void> &obs) {
  class awaiter {
  public:
    explicit awaiter(observable<> &obs) : obs_{obs} {}

    bool await_ready() const { return false; }

    bool await_suspend(std::experimental::coroutine_handle<> resume_cb) {
      obs_.subscribe(sub, [&resume_cb]() {
		  resume_cb(); 
	  });
	  // TODO special case with cancellations?
	  return true;	// always suspend
    }

    void await_resume() {}

    ~awaiter() {}

  private:
	subscription sub;
    observable<> &obs_;
  };

  return awaiter{obs};
}

// TODO special version with cancellations