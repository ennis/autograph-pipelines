#pragma once
#include <algorithm>
#include <functional>
#include <memory>
#include <vector>

// TODO clean this up, this is an unholy mess
namespace ag {
class subscription_impl {};

class Subscription {
  template <typename T> friend class Observable;

public:
  void unsubscribe() { ptr_.reset(); }

private:
  std::shared_ptr<subscription_impl> ptr_ =
      std::make_shared<subscription_impl>();
};

template <typename T> class Observable;

///////////////////////////////////////////////////////////////
class Scheduler {
public:
  template <typename T> void schedule(Observable<T> obs, T &&value);
  template <typename T> void schedule(Observable<T> obs);

  void execute() {
    for (auto &&d : deferred_)
      d();
    deferred_.clear();
  }

private:
  std::vector<std::function<void()>> deferred_;
};

namespace detail {
template <typename U> struct callback {
  std::function<void(U)> fn_;
  std::weak_ptr<subscription_impl> sub_;
};

template <> struct callback<void> {
  std::function<void()> fn_;
  std::weak_ptr<subscription_impl> sub_;
};
}

///////////////////////////////////////////////////////////////
// cancellable observables
template <typename T = void> class Observable {
public:
  friend class Scheduler;
  using type = T;

  Observable() : ptr_{std::make_shared<state>()} {}

  // Observable(const Observable &) = delete;
  // Observable &operator=(const Observable &) = delete;
  // TODO make them non-movable
  // Observable(Observable &&rhs) = default;
  // Observable &operator=(Observable &&) = default;

  template <typename Func> void subscribe(Subscription &sub, Func &&func) {
    ptr_->subscribe(sub, std::forward<Func>(func));
  }
  template <typename Func> void subscribe(Func &&func) {
    ptr_->subscribe(std::forward<Func>(func));
  }

  template <typename U = T>
  void operator()(std::enable_if_t<!std::is_void<U>::value, U> value) {
    ptr_->signal(value);
  }

  template <typename U = T>
  std::enable_if_t<std::is_void<U>::value> operator()() {
    ptr_->signal();
  }

  template <typename U = T>
  void signal_deferred(Scheduler &sched,
                       std::enable_if_t<!std::is_void<U>::value, U> value) {
    sched.schedule(*this, std::forward(value));
  }

  template <typename U = T>
  std::enable_if_t<std::is_void<U>::value> signal_deferred(Scheduler &sched) {
    sched.schedule(*this);
  }

protected:
  struct state {
    void reap() {
      callbacks_.erase(
          std::remove_if(callbacks_.begin(), callbacks_.end(),
                         [](const auto &c) { return c.sub_.expired(); }),
          callbacks_.end());
    }

    template <typename Func> void subscribe(Subscription &sub, Func &&func) {
      callbacks_.push_back(
          detail::callback<T>{std::forward<Func>(func), sub.ptr_});
    }

    template <typename Func> void subscribe(Func &&func) {
      callbacks_.push_back(
          detail::callback<T>{std::forward<Func>(func), sub_.ptr_});
    }

    // see http://stackoverflow.com/questions/2892087/
    template <typename U = T>
    void signal(std::enable_if_t<!std::is_void<U>::value, U> value) {
      auto init_size = callbacks_.size();
      for (int i = 0; i < init_size; ++i)
        if (auto sp = callbacks_[i].sub_.lock())
          callbacks_[i].fn_(value);
      // ISSUE: one of the callbacks may resume a coroutine that owns the
      // Observable pointed by 'this'
      // If the coroutine ends, this observable is destroyed but the execution
      // still continues here in signal()
      // Solution: use a shared_ptr? make a copy of callbacks_?
      //
      // TL;DR: invoking a callback may destroy the current observable.
      reap();
    }

    template <typename U = T>
    std::enable_if_t<std::is_void<U>::value> signal() {
      // do not use iterators: they can be invalidated by callbacks
      auto init_size = callbacks_.size();
      for (int i = 0; i < init_size; ++i)
        if (auto sp = callbacks_[i].sub_.lock())
          callbacks_[i].fn_();
      reap();
    }

    template <typename U = T>
    void signal_deferred(Scheduler &sched,
                         std::enable_if_t<!std::is_void<U>::value, U> value) {
      sched.schedule(*this, std::forward(value));
    }

    template <typename U = T>
    std::enable_if_t<std::is_void<U>::value> signal_deferred(Scheduler &sched) {
      sched.schedule(*this);
    }

    std::vector<detail::callback<T>> callbacks_;
    Subscription sub_;
  };

  std::shared_ptr<state> ptr_;
};

template <typename T> void Scheduler::schedule(Observable<T> obs, T &&value) {
  deferred_.emplace_back(
      [ obs{std::move(obs)}, value{std::move(value)} ]() mutable {
        obs.signal(value);
      });
}

template <typename T> void Scheduler::schedule(Observable<T> obs) {
  deferred_.emplace_back([obs{std::move(obs)}]() mutable { obs.signal(); });
}

///////////////////////////////////////////////////////////////
///
class observer_base {
protected:
  Subscription sub_;

public:
  template <typename T, typename Func>
  void listen(Observable<T> &src, Func &&func) {
    src.subscribe(sub_, std::forward<Func>(func));
  }
};

template <typename Derived> class observer : public observer_base {
public:
  template <typename T>
  void listen(Observable<T> &src, void (Derived::*member)(T)) {
    src.subscribe(sub_, [this, member](T &&v) {
      static_cast<Derived *>(this)->member(std::forward(v));
    });
  }

  void listen(Observable<void> &src, void (Derived::*member)()) {
    src.subscribe(
        sub_, [this, member]() { (static_cast<Derived *>(this)->*member)(); });
  }
};

///////////////////////////////////////////////////////////////
///
/*template <typename... Types>
struct multi_observable : public Observable<eggs::variant<Types...>> {
  // attach an observer to each observable?
  // each observer has its own type
  // use for_each_in_tuple?

  ~multi_observable() { sub_.unsubscribe(); }

  void attach(const std::tuple<Observable<Types> &...> &observables) {
    for_each_in_tuple(observables_, [this](auto &&obs) {
      obs.subscribe(this->sub_, [this](decltype(obs)::type v) {
        this->signal(eggs::variant<Types...>(std::move(v)));
      });
    });
  }

  Subscription sub_;
};*/

// error(empty) or T (optional type)
/*template <typename T = void> struct result {
  result() = default;
  result(T &&v) : v_{std::move(v)} {}

  bool has_error() const { return v_.empty(); }
  std::experimental::optional<T> v_;
};*/

/*template <> struct result<void> {
  result() = default;
  result(bool error) : has_error_{error} {}

  bool has_error() const { return has_error_; }
  bool has_error_{true};

  static result<void> ok() { return result<void>{false}; }
  static result<void> error() { return result<void>{true}; }
};*/

/*class cancellation_token : public Observable<> {};

template <typename T> class combined_observable : public Observable<result<T>> {
public:
  Subscription sub_;
};

template <typename T>
auto operator|(Observable<T> obs, cancellation_token cancel) {
  struct combined_observable : public Observable<result<T>> {
    Subscription sub_;
  } combined_obs;
  obs.subscribe(combined_obs.sub_, [combined_obs](auto &&v) mutable {
    combined_obs(result<T>{std::move(v)});
  });
  cancel.subscribe(combined_obs.sub_,
                   [combined_obs]() mutable { combined_obs(result<T>{}); });
  return combined_obs;
}

inline auto operator|(Observable<void> obs, cancellation_token cancel) {
  struct combined_observable : public Observable<result<>> {
    Subscription sub_;
  } combined_obs;
  obs.subscribe(combined_obs.sub_,
                [combined_obs]() mutable { combined_obs(result<void>::ok()); });
  cancel.subscribe(combined_obs.sub_, [combined_obs]() mutable {
    combined_obs(result<void>::error());
  });
  return combined_obs;
}*/

// TODO: operator | (obs<T>, obs<U>) -> Observable<variant<T,U>>
//    operator | (obs<T>, cancellation) -> Observable<result<T>>
//    operator | (obs<result<variant<T...>>, obs<result<U...>>) ->
//    Observable<result<T..., U...>>
//    await(obs<result<T>>) -> returns T or cancels
//
// combine obs<T>, obs<variant<T...>>, obs<result<variant<T...>>
}