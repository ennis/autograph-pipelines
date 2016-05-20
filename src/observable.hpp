#pragma once
#include <functional>
#include <memory>
#include <vector>

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

template <typename T = void> class observable {
public:
  // disable copy
  observable() = default;
  observable(const observable &) = delete;
  observable &operator=(const observable &) = delete;
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
