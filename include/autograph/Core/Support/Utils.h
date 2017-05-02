#pragma once
#include <tuple>
#include <type_traits>
#include <utility>

////////////////////////// Tuple unpack utils
//////////////////////////
/// http://stackoverflow.com/questions/7858817/unpacking-a-tuple-to-call-a-matching-function-pointer

namespace ag {

// helper class
template <typename F, typename... Args, std::size_t... I>
auto call_helper(const F &func, const std::tuple<Args...> &params,
                 std::index_sequence<I...>) -> std::result_of_t<F(Args...)> {
  return func(std::get<I>(params)...);
}

template <typename F, typename... Args>
auto call(const F &func, const std::tuple<Args...> &params)
    -> std::result_of_t<F(Args...)> {
  return call_helper<F, Args...>(func, params,
                                 std::index_sequence_for<Args...>());
}

// https://rmf.io/cxx11/overload-ranking/
struct overload_otherwise {
  overload_otherwise(...) {}
};

template <unsigned I> struct overload_choice : overload_choice<I + 1> {};
// terminate recursive inheritence at a convenient point,
// large enough to cover all cases
template <> struct overload_choice<10> {};

// "for clarity"
struct select_overload : overload_choice<0> {};
}
