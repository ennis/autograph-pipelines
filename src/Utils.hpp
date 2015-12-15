#ifndef UTILS_HPP
#define UTILS_HPP

#include <type_traits>
#include <utility>

////////////////////////// Tuple unpack utils
////////////////////////// http://stackoverflow.com/questions/7858817/unpacking-a-tuple-to-call-a-matching-function-pointer

namespace util
{
	// helper class
	template <
		typename F,
		typename... Args,
		std::size_t... I
	>
	auto call_helper(const F& func, const std::tuple<Args...> & params) -> std::result_of_t<F(Args...)>
	{
		return func(std::get<I>(params)...);
	}

	template<
		typename F,
		typename... Args
	>
	auto call(const F& func, const std::tuple<Args...> & params) -> std::result_of_t<F(Args...)>
	{
		return call_helper<F, Args..., std::index_sequence_for<Args...> >(func, params);
	}
}

////////////////////////// Bitflags enumerations
template <typename T> struct is_enum_flags : std::false_type
{
	//static_assert(std::is_enum<T>::value, "T must be an enum type");
};

////////////////////////// operator| 
template <
	typename T,
	typename = std::enable_if_t<is_enum_flags<T>::value>
>
inline T operator|(T a, T b)
{
	return static_cast<T>(static_cast<std::underlying_type_t<T>>(a) | static_cast<std::underlying_type_t<T>>(b));
}

////////////////////////// operator|=
template <
	typename T,
	typename = std::enable_if_t<is_enum_flags<T>::value>
>
inline T& operator|=(T& a, T b)
{
	a = a | b;
	return a;
}

////////////////////////// operator&=
template <
	typename T,
	typename = std::enable_if_t<is_enum_flags<T>::value>
>
inline T operator&(T a, T b)
{
	return static_cast<T>(static_cast<std::underlying_type_t<T>>(a) & static_cast<std::underlying_type_t<T>>(b));
}

#endif // !UTILS_HPP
