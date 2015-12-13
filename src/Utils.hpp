#ifndef UTILS_HPP
#define UTILS_HPP

#include <type_traits>

// Bitflags enumerations
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
