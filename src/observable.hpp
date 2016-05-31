#pragma once
#include <algorithm>
#include <functional>
#include <memory>
#include <vector>

#include "eggs/variant.hpp"

class subscription_impl {};

class subscription {
  template <typename T> friend class observable;

public:
  void unsubscribe() { ptr_.reset(); }

private:
  std::shared_ptr<subscription_impl> ptr_ =
      std::make_shared<subscription_impl>();
};

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

class scheduler {
public:
  template <typename T> void schedule(observable<T> &obs, T &&value) {
    // capture obs by ref, move value into closure
    deferred_.emplace_back(
        [&obs, value{std::move(value)} ]() { obs.signal(value); });
  }

  template <typename T> void schedule(observable<T> &obs) {
    // capture obs by ref, move value into closure
    deferred_.emplace_back([&obs]() { obs.signal(); });
  }

  void execute() {
    for (auto &&d : deferred_)
      d();
    deferred_.clear();
  }

  std::vector<std::function<void()>> deferred_;
};

template <typename T = void> class observable {
public:
	using type = T;

  // disable copy
  observable() = default;
  observable(const observable &) = delete;
  observable &operator=(const observable &) = delete;
  // TODO make them non-movable
  observable(observable &&rhs) = default;
  observable &operator=(observable &&) = default;

  template <typename Func> void subscribe(subscription &sub, Func &&func) {
    callbacks_.push_back(callback{std::forward<Func>(func), sub.ptr_});
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

  template <typename U = T> std::enable_if_t<std::is_void<U>::value> signal() {
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

private:
  void reap() {
    callbacks_.erase(
        std::remove_if(callbacks_.begin(), callbacks_.end(),
                       [](const auto &c) { return c.sub_.expired(); }),
        callbacks_.end());
  }

  struct callback {
    std::function<void(T)> fn_;
    std::weak_ptr<subscription_impl> sub_;
  };

  std::vector<callback> callbacks_;
};


// awaitable type (but not observable)
template <typename ... Types>
struct multi_observable : public observable<eggs::variant<Types...>>
{
	// attach an observer to each observable?
	// each observer has its own type
	// use for_each_in_tuple?

	~multi_observable()
	{
		sub_.unsubscribe();
	}
	
	void attach(const std::tuple<observable<Types>&...>& observables)
	{
		for_each_in_tuple(observables_, [this](auto&& obs) {
			obs.subscribe(this->sub_, [this](decltype(obs)::type v) {
				this->signal(eggs::variant<Types...>(std::move(v)));
			});
		});
	}

	subscription sub_;
};

class cancellation_token : public observable<>
{

};

// TODO: operator | (obs<T>, obs<U>) -> observable<variant<T,U>>
//		operator | (obs<T>, cancellation) -> observable<result<T>>
// 
