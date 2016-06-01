#pragma once
#include <algorithm>
#include <functional>
#include <memory>
#include <vector>

#include "eggs/variant.hpp"
#include "optional.hpp"

class subscription_impl {};

class subscription {
  template <typename T> friend class observable;

public:
  void unsubscribe() { ptr_.reset(); }

private:
  std::shared_ptr<subscription_impl> ptr_ =
      std::make_shared<subscription_impl>();
};

template <typename T>
class observable;

///////////////////////////////////////////////////////////////
class scheduler {
public:
  template <typename T> void schedule(observable<T> &obs, T &&value);
  template <typename T> void schedule(observable<T> &obs);

  void execute() {
    for (auto &&d : deferred_)
      d();
    deferred_.clear();
  }

private:
  std::vector<std::function<void()>> deferred_;
};

namespace detail
{
template <typename U>
struct callback {
  std::function<void(U)> fn_;
  std::weak_ptr<subscription_impl> sub_;
};

template <>
struct callback<void> {
  std::function<void()> fn_;
  std::weak_ptr<subscription_impl> sub_;
};
}

///////////////////////////////////////////////////////////////
// cancellable observables
template <typename T = void> class observable {
public:
  friend class scheduler;
  using type = T;

  // disable copy
  observable() : ptr_{std::make_unique<state>()}
  {}

  observable(const observable &) = delete;
  observable &operator=(const observable &) = delete;
  // TODO make them non-movable
  observable(observable &&rhs) = default;
  observable &operator=(observable &&) = default;

  template <typename Func> void subscribe(subscription &sub, Func &&func) {
    ptr_->subscribe(sub, std::forward<Func>(func));
  }

  template <typename U = T>
  void signal(std::enable_if_t<!std::is_void<U>::value, U> value) {
    ptr_->signal(value);
  }

  template <typename U = T> std::enable_if_t<std::is_void<U>::value> signal() {
    ptr_->signal();
  }

  template <typename U = T>
  void signal_deferred(scheduler &sched,
                       std::enable_if_t<!std::is_void<U>::value, U> value) {
    sched.schedule(*this, std::forward(value));
  }

  template <typename U = T>
  std::enable_if_t<std::is_void<U>::value> signal_deferred(scheduler &sched) {
    sched.schedule(*this);
  }

private:


  struct state {
    void reap() {
      callbacks_.erase(
          std::remove_if(callbacks_.begin(), callbacks_.end(),
                         [](const auto &c) { return c.sub_.expired(); }),
          callbacks_.end());
    }

    template <typename Func> void subscribe(subscription &sub, Func &&func) {
      callbacks_.push_back(detail::callback<T>{std::forward<Func>(func), sub.ptr_});
    }

    // see http://stackoverflow.com/questions/2892087/
    template <typename U = T>
    void signal(std::enable_if_t<!std::is_void<U>::value, U> value) {
      auto init_size = callbacks_.size();
      for (int i = 0; i < init_size; ++i)
        if (auto sp = callbacks_[i].sub_.lock())
          callbacks_[i].fn_(value);
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
    void signal_deferred(scheduler &sched,
                         std::enable_if_t<!std::is_void<U>::value, U> value) {
      sched.schedule(*this, std::forward(value));
    }

    template <typename U = T>
    std::enable_if_t<std::is_void<U>::value> signal_deferred(scheduler &sched) {
      sched.schedule(*this);
    }

    std::vector<detail::callback<T>> callbacks_;
  };

  std::unique_ptr<state> ptr_;
};

template <typename T>
void scheduler::schedule(observable<T> &obs, T &&value) {
  deferred_.emplace_back([ptr = obs.ptr_.get(), value{std::move(value)}]() {
    ptr->signal(value);
  });
}

template <typename T>
void scheduler::schedule(observable<T> &obs) {
  deferred_.emplace_back([ptr = obs.ptr_.get()]() { ptr->signal(); });
}

///////////////////////////////////////////////////////////////
///
class observer_base {
protected:
  subscription sub_;

public:
  template <typename T, typename Func>
  void listen(observable<T> &src, Func &&func) {
    src.subscribe(sub_, std::forward<Func>(func));
  }
};

template <typename Derived> class observer : public observer_base {
public:
  template <typename T>
  void listen(observable<T> &src, void (Derived::*member)(T)) {
    src.subscribe(sub_, [this, member](T &&v) {
      static_cast<Derived *>(this)->member(std::forward(v));
    });
  }

  void listen(observable<void> &src, void (Derived::*member)()) {
    src.subscribe(
        sub_, [this, member]() { (static_cast<Derived *>(this)->*member)(); });
  }
};

///////////////////////////////////////////////////////////////
///
/*template <typename... Types>
struct multi_observable : public observable<eggs::variant<Types...>> {
  // attach an observer to each observable?
  // each observer has its own type
  // use for_each_in_tuple?

  ~multi_observable() { sub_.unsubscribe(); }

  void attach(const std::tuple<observable<Types> &...> &observables) {
    for_each_in_tuple(observables_, [this](auto &&obs) {
      obs.subscribe(this->sub_, [this](decltype(obs)::type v) {
        this->signal(eggs::variant<Types...>(std::move(v)));
      });
    });
  }

  subscription sub_;
};*/

// error(empty) or T (optional type)
template <typename T = void> struct result {
  bool has_error() const { return v_.empty(); }
  std::experimental::optional<T> v_;
};

template <> struct result<void> {
  bool has_error() const { return has_error_; }
  bool has_error_;
};

class cancellation_token : public observable<result<>> {};

template <typename T>
auto operator|(observable<T> &obs, cancellation_token &cancel) {
  class combined_observable : public observable<T> {
  public:
    // issue: I need to signal this observable, so I need to
    // keep a reference to it in the callbacks that are passed
    // to 'obs' and 'cancel'
    // BUT the address of this observable may change (because it can be moved)
    // when returned from a function, thus invalidating the references.
    //
    // Solution: use a shared_ptr/unique_ptr internally
    // -> Issue: heap allocation
    // -> -> Mitigation: object pools
    //  ** best solution **
    //
    // Solution 2: do not attach until the observable has finished moving
    //    can't do it in ctor, must keep ref to obs and cancel
    //
    // Solution 3: on move, detach and reattach
    //    no

  private:
    subscription sub_;
  };
}

// TODO: operator | (obs<T>, obs<U>) -> observable<variant<T,U>>
//    operator | (obs<T>, cancellation) -> observable<result<T>>
//    operator | (obs<result<variant<T...>>, obs<result<U...>>) ->
//    observable<result<T..., U...>>
//    await(obs<result<T>>) -> returns T or cancels
//
// combine obs<T>, obs<variant<T...>>, obs<result<variant<T...>>
