#pragma once
#include <type_traits>
#include <utility>

#define ENUM_BIT_FLAGS_OPERATORS(type)                                         \
                                                                               \
  constexpr inline type operator|(type a, type b) {                            \
    return static_cast<type>(static_cast<std::underlying_type_t<type>>(a) |    \
                             static_cast<std::underlying_type_t<type>>(b));    \
  }                                                                            \
                                                                               \
  constexpr inline type operator&(type a, type b) {                            \
    return static_cast<type>(static_cast<std::underlying_type_t<type>>(a) &    \
                             static_cast<std::underlying_type_t<type>>(b));    \
  }                                                                            \
                                                                               \
  inline type &operator|=(type &a, type b) {                                   \
    a = a | b;                                                                 \
    return a;                                                                  \
  }                                                                            \
                                                                               \
  inline type &operator&=(type &a, type b) {                                   \
    a = a & b;                                                                 \
    return a;                                                                  \
  }                                                                            \
                                                                               \
  constexpr inline bool not_empty(type a) {                                    \
    return static_cast<std::underlying_type_t<type>>(a) != 0;                  \
  }                                                                            \
  constexpr inline bool operator!(type a) {                                    \
    return static_cast<std::underlying_type_t<type>>(a) == 0;                  \
  }
