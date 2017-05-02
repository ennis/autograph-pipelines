#pragma once
#include <autograph/Core/Exports.h>
#include <autograph/Core/Types.h>
#include <fmt/format.h>
#include <ostream>

//
// Debug & logging utilities
// Don't include that in public headers please
//

// formatting operators for vector types

inline std::ostream& operator<<(std::ostream& os, ag::vec2 v) {
	os << '<' << v.x << ',' << v.y << '>';
	return os;
}
inline std::ostream& operator<<(std::ostream& os, ag::vec3 v) {
	os << '<' << v.x << ',' << v.y << ',' << v.z << '>';
	return os;
}
inline std::ostream& operator<<(std::ostream& os, ag::vec4 v) {
	os << '<' << v.x << ',' << v.y << ',' << v.z << ',' << v.w << '>';
	return os;
}
inline std::ostream& operator<<(std::ostream& os, ag::ivec2 v) {
	os << '<' << v.x << ',' << v.y << '>';
	return os;
}
inline std::ostream& operator<<(std::ostream& os, ag::ivec3 v) {
	os << '<' << v.x << ',' << v.y << ',' << v.z << '>';
	return os;
}
inline std::ostream& operator<<(std::ostream& os, ag::ivec4 v) {
	os << '<' << v.x << ',' << v.y << ',' << v.z << ',' << v.w << '>';
	return os;
}

#include <fmt/ostream.h>

namespace ag {


enum class LogLevel { Debug, Warning, Error, Fatal };

AG_CORE_API void rawLogMessage(LogLevel c, const char *message);

template <typename... Args>
void logMessage(LogLevel c, const char *message, Args &&... args) {
  auto str = fmt::format(message, std::forward<Args>(args)...);
  rawLogMessage(c, str.c_str());
}

template <typename... Args> void debugMessage(const char *message, Args &&... args)
{
	logMessage(LogLevel::Debug, message, std::forward<Args>(args)...);
}

template <typename... Args> 
void warningMessage(const char *message, Args &&... args)
{
	logMessage(LogLevel::Warning, message, std::forward<Args>(args)...);
}

template <typename... Args> 
void errorMessage(const char *message, Args &&... args)
{
	logMessage(LogLevel::Error, message, std::forward<Args>(args)...);
}

// Fail with a message and call std::terminate
[[noreturn]] AG_CORE_API void failWith(const char *message);
[[noreturn]] AG_CORE_API void failWith(const std::string &message);

#ifdef AG_DEBUG_OUTPUT
#define AG_DEBUG(...) ::ag::debugMessage(__VA_ARGS__)
#else
#define AG_DEBUG(...)
#endif

//
// If DEBUG=ON then show debug messages
// else, show only NOTE, WARNING and ERROR messages

}
