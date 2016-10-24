#pragma once
#include "Observable.h"
#include <experimental/coroutine>

// TODO clean this up, this is an unholy mess
namespace ag {

template <typename T> class ObservableAwaiterBase {
public:
  explicit ObservableAwaiterBase(Observable<T> &obs) : obs_{obs} {}
  bool await_ready() const { return false; }
  void await_resume() {}
protected:
  Subscription sub;
  Observable<T> &obs_;
};

template <typename T>
class ObservableAwaiter : public ObservableAwaiterBase<T> {
public:
  using ObservableAwaiterBase<T>::ObservableAwaiterBase;
  bool await_suspend(std::experimental::coroutine_handle<> resume_cb) {
    obs_.subscribe(sub, [resume_cb]() { resume_cb(); });
    return true;
  }
};

/*template <typename U>
class ObservableAwaiter<result<U>> : public ObservableAwaiterBase<result<U>> {
public:
  using ObservableAwaiterBase<result<U>>::ObservableAwaiterBase;
  bool await_suspend(std::experimental::coroutine_handle<> resume_cb) {
    obs_.subscribe(sub, [resume_cb] (result<U> res) mutable {
      if (res.has_error())
        resume_cb.destroy(); // will destroy variables with automatic storage
      else
        resume_cb(); // TODO also set promise return value
    });
    return true; // always suspend
  }
};*/

template <typename T> inline auto operator co_await(Observable<T> &obs) {
  return ObservableAwaiter<T>{obs};
}

}