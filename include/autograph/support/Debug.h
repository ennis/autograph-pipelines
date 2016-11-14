#pragma once
#include <autograph/Config.h>
#include <fmt/format.h>

//
// Debug & logging utilities
// Don't include that in public headers please
//

namespace ag {

enum class LogLevel { Debug, Warning, Error, Fatal };

AG_API void rawLogMessage(LogLevel c, const char *message);

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
[[noreturn]] AG_API void failWith(const char *message);
[[noreturn]] AG_API void failWith(const std::string &message);

#ifdef AG_DEBUG_OUTPUT
#define AG_DEBUG(...) ::ag::debugMessage(__VA_ARGS__)
#else
#define AG_DEBUG(...)
#endif

//
// If DEBUG=ON then show debug messages
// else, show only NOTE, WARNING and ERROR messages

}