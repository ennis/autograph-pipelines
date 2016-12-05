#ifdef AG_WINDOWS
#include <Windows.h>
#endif
#include <autograph/support/Debug.h>
#include <exception>
#include <iostream>

namespace ag {

static void setConsoleColor(std::ostream& os, int attrib) {
#ifdef AG_WINDOWS
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  SetConsoleTextAttribute(hConsole, attrib);
#else
    switch (attrib) {
    case 8: os << "\033[30m"; break;
    case 7: os << "\033[37m"; break;
    case 14: os << "\033[33m"; break;
    case 12: os << "\033[31m"; break;
    }
#endif
}

AG_API void rawLogMessage(LogLevel c, const char *message) {
  if (c == LogLevel::Debug) {
    setConsoleColor(std::cerr, 8);
    std::cerr << "[DEBUG] ";
  } else if (c == LogLevel::Warning) {
    setConsoleColor(std::cerr, 14);
    std::cerr << "[WARN ] ";
    setConsoleColor(std::cerr, 7);
  } else if (c == LogLevel::Error) {
    setConsoleColor(std::cerr, 12);
    std::cerr << "[ERROR] ";
    setConsoleColor(std::cerr, 7);
  } else if (c == LogLevel::Fatal) {
    setConsoleColor(std::cerr, 12);
    std::cerr << "\n[FATAL] ";
  }
  std::cerr << message << "\n";
  setConsoleColor(std::cerr, 7);
}

[[noreturn]] void failWith(const char *message) {
  rawLogMessage(LogLevel::Fatal, message);
  std::terminate();
}

[[noreturn]] void failWith(const std::string &message) {
  failWith(message.c_str());
}
}
