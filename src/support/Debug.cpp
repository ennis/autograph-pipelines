#ifdef AG_WINDOWS
#include <Windows.h>
#endif
#include <autograph/support/Debug.h>
#include <exception>
#include <iostream>

namespace ag {

static void setConsoleColor(int attrib) {
#ifdef AG_WINDOWS
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  SetConsoleTextAttribute(hConsole, attrib);
#endif
}

AG_API void rawLogMessage(LogLevel c, const char *message) {
  if (c == LogLevel::Debug) {
    std::cerr << "[DEBUG] ";
  } else if (c == LogLevel::Warning) {
    setConsoleColor(14);
    std::cerr << "[WARN ] ";
    setConsoleColor(7);
  } else if (c == LogLevel::Error) {
    setConsoleColor(12);
    std::cerr << "[ERROR] ";
    setConsoleColor(7);
  } else if (c == LogLevel::Fatal) {
    setConsoleColor(12);
    std::cerr << "\n\n[FATAL] ";
  }
  std::cerr << message << "\n";
  setConsoleColor(7);
}

[[noreturn]] void failWith(const char *message) {
  rawLogMessage(LogLevel::Fatal, message);
  std::terminate();
}

[[noreturn]] void failWith(const std::string &message) {
  failWith(message.c_str());
}
}
