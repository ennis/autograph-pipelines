#pragma once
#include "optional.hpp"
#include <support/Utils.hpp>

// loader that just calls the constructor
template <typename T> struct lazy_ctor {
public:
  template <typename... Args> T operator()(Args &&... args) {
    return T{std::forward<Args>(args)...};
  }

private:
};

template <typename T, typename Loader = lazy_ctor<T>, typename... Args>
class lazy_resource {
public:
  lazy_resource(Args &&... args) : args_{args...} {}

  template <typename ... AdditionalArgs>
  T &get(AdditionalArgs&&... args) {
    if (!res_)
      do_load(std::forward(args)...);
    return *res_;
  }

private:
	template <typename ... AdditionalArgs>
  void do_load(AdditionalArgs&&... additional_args) {
    using namespace boost;
	res_ = util::call([&](auto... args) { return Loader{}(args..., additional_args...); }, args_);
  }

  std::experimental::optional<T> res_;
  std::tuple<Args...> args_;
};

template <typename T, typename Loader = lazy_ctor<T>, typename... Args>
lazy_resource<T, Loader, Args...> make_lazy_resource(Args &&... args) {
  return lazy_resource<T, Loader, Args...>(std::forward<Args>(args)...);
}