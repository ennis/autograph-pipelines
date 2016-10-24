#pragma once
#include "coroutine.hpp"
#include "observable.hpp"

// await: creates a callable that resumes the current coroutine/thread,
// subscribe to the future and suspends the current coroutine/thread
template <typename T> T await(Observable<T> &obs) {
  T value_tmp;
  auto ts = Coroutine::current();
  if (!ts) {
    std::clog << "ABORT: await() called outside a resumable task\n";
    std::terminate();
  }
  Subscription sub;
  obs.subscribe(sub, [&value_tmp, ts](const T &value) {
    value_tmp = value;
    ts->resume();
  });
  // TODO what if the observable triggers here?
  // in this case, the task is not yet suspended
  // and will resume here, causing an abort (in task_state::resume)
  Coroutine::suspend();
  return value_tmp;
}

inline void await(Observable<void> &obs) {
  auto ts = Coroutine::current();
  if (!ts) {
    std::clog << "ABORT: await() called outside a resumable task\n";
    std::terminate();
  }
  Subscription sub;
  obs.subscribe(sub, [ts]() { ts->resume(); });
  Coroutine::suspend();
}

//////////////////////////////////////////////////////////////////////////
/*template <typename T> class observable_awaiter_base {
public:
  explicit observable_awaiter_base(Observable<T> &obs) : obs_{obs} {}
  bool await_ready() const { return false; }
  void await_resume() {}

protected:
  Subscription sub;
  Observable<T> &obs_;
};

template <typename T>
class observable_awaiter : public observable_awaiter_base<T> {
public:
  using observable_awaiter_base<T>::observable_awaiter_base;

  bool await_suspend(std::experimental::coroutine_handle<> resume_cb) {
    obs_.subscribe(sub, [resume_cb]() { resume_cb(); });
    return true;
  }
};

template <typename U>
class observable_awaiter<result<U>> : public observable_awaiter_base<result<U>> {
public:
  using observable_awaiter_base<result<U>>::observable_awaiter_base;

  bool await_suspend(std::experimental::coroutine_handle<> resume_cb) {
    obs_.subscribe(sub, [resume_cb] (result<U> res) mutable {
      if (res.has_error())
        resume_cb.destroy(); // will destroy variables with automatic storage
      else
        resume_cb(); // TODO also set promise return value
    });
    return true; // always suspend
  }
};

template <typename T> inline auto operator co_await(Observable<T> &obs) {
  return observable_awaiter<T>{obs};
}*/
