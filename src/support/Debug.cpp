#ifdef AG_WINDOWS
#include <Windows.h>
#endif
#include <autograph/support/Debug.h>
#include <exception>
#include <iostream>

namespace ag {

template <typename ResultT, ResultT OffsetBasis, ResultT Prime>
class basic_fnv1a final {
  static_assert(std::is_unsigned<ResultT>::value, "need unsigned integer");

public:
  using result_type = ResultT;

private:
  result_type state_{};

public:
  basic_fnv1a() noexcept : state_{OffsetBasis} {}

  void update(const void *const data, const std::size_t size) noexcept {
    const auto cdata = static_cast<const unsigned char *>(data);
    auto acc = this->state_;
    for (auto i = std::size_t{}; i < size; ++i) {
      const auto next = std::size_t{cdata[i]};
      acc = (acc ^ next) * Prime;
    }
    this->state_ = acc;
  }

  result_type digest() const noexcept { return this->state_; }
};

using fnv1a_32 =
    basic_fnv1a<std::uint32_t, UINT32_C(2166136261), UINT32_C(16777619)>;

using fnv1a_64 = basic_fnv1a<std::uint64_t, UINT64_C(14695981039346656037),
                             UINT64_C(1099511628211)>;

inline std::size_t hash_bytes(const void *const data,
                              const std::size_t size) noexcept {
  auto hashfn = fnv1a_64{};
  hashfn.update(data, size);
  return hashfn.digest();
}

static void setConsoleColor(std::ostream &os, int attrib) {
#ifdef AG_WINDOWS
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  SetConsoleTextAttribute(hConsole, attrib);
#else
  switch (attrib) {
  case 8:
    os << "\033[30m";
    break;
  case 7:
    os << "\033[37m";
    break;
  case 14:
    os << "\033[33m";
    break;
  case 12:
    os << "\033[31m";
    break;
  }
#endif
}

static void rawDebugOutput(int timesRepeated, const char *message) {
  // do not print the message again if it is repeated
  if (!timesRepeated)
    std::cerr << message;
#ifdef AG_WINDOWS
  // Always repeat
  OutputDebugStringA(message);
#endif
}

static void rawRepeatLast(int timesRepeated, const char *message) {
  std::cerr << "\r    (Repeated " << timesRepeated << " times)";
}

AG_API void rawLogMessage(LogLevel c, const char *message) {
  static std::size_t lastHash = 0;
  static int timesRepeated = 0;
  std::size_t curHash =
      hash_bytes(message, std::strlen(message)) + (std::size_t)c;
  // bool repeatLast = false;
  if (curHash == lastHash) {
    // repeat last output
    ++timesRepeated;
    setConsoleColor(std::cerr, 8);
    rawRepeatLast(timesRepeated, message);
    setConsoleColor(std::cerr, 7);
  } else {
    lastHash = curHash;
    if (timesRepeated)
      rawDebugOutput(0, "\n");
    timesRepeated = 0;
  }

  if (c == LogLevel::Debug) {
    setConsoleColor(std::cerr, 8);
    rawDebugOutput(timesRepeated, "[DEBUG] ");
  } else if (c == LogLevel::Warning) {
    setConsoleColor(std::cerr, 14);
    rawDebugOutput(timesRepeated, "[WARN ] ");
    setConsoleColor(std::cerr, 7);
  } else if (c == LogLevel::Error) {
    setConsoleColor(std::cerr, 12);
    rawDebugOutput(timesRepeated, "[ERROR] ");
    setConsoleColor(std::cerr, 7);
  } else if (c == LogLevel::Fatal) {
    setConsoleColor(std::cerr, 12);
    rawDebugOutput(timesRepeated, "\n[FATAL] ");
  }
  rawDebugOutput(timesRepeated, message);
  rawDebugOutput(timesRepeated, "\n");
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
