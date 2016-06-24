#ifndef UTILS_HPP
#define UTILS_HPP

#include <tuple>
#include <type_traits>
#include <utility>

////////////////////////// Tuple unpack utils
//////////////////////////
/// http://stackoverflow.com/questions/7858817/unpacking-a-tuple-to-call-a-matching-function-pointer

namespace util {
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
}

#define ENUM_BIT_FLAGS_OPERATORS(type)                                         \
  \
inline type                                                                    \
  operator|(type a, type b) {                                                  \
    return static_cast<type>(static_cast<std::underlying_type_t<type>>(a) |    \
                             static_cast<std::underlying_type_t<type>>(b));    \
  }                                                                            \
  \
inline type                                                                    \
  operator&(type a, type b) {                                                  \
    return static_cast<type>(static_cast<std::underlying_type_t<type>>(a) &    \
                             static_cast<std::underlying_type_t<type>>(b));    \
  }                                                                            \
  \
inline type &                                                                  \
  operator|=(type &a, type b) {                                                \
    a = a | b;                                                                 \
    return a;                                                                  \
  }                                                                            \
  \
inline type &                                                                  \
  operator&=(type &a, type b) {                                                \
    a = a & b;                                                                 \
    return a;                                                                  \
  }                                                                            \
  \
inline bool                                                                    \
  not_empty(type a) {                                                          \
    return static_cast<std::underlying_type_t<type>>(a) != 0;                  \
  }                                                                            \
inline bool                                                                    \
  operator!(type a) {                                                          \
return static_cast<std::underlying_type_t<type>>(a) != 0;                      \
}

// http://stackoverflow.com/questions/16387354/template-tuple-calling-a-function-on-each-element
namespace detail {
template <int... Is> struct seq {};
template <int N, int... Is> struct gen_seq : gen_seq<N - 1, N - 1, Is...> {};
template <int... Is> struct gen_seq<0, Is...> : seq<Is...> {};
}

namespace detail {
template <typename T, typename F, int... Is>
void for_each(T &&t, F f, seq<Is...>) {
  auto l = {(f(std::get<Is>(t)), 0)...};
}
}

template <typename... Ts, typename F>
void for_each_in_tuple(std::tuple<Ts...> const &t, F f) {
  detail::for_each(t, f, detail::gen_seq<sizeof...(Ts)>());
}

#endif // !UTILS_HPP
