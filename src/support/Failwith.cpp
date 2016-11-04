#include <autograph/support/Failwith.h>
#include <iostream>
#include <exception>

namespace ag {
[[noreturn]] void failWith(const char *message) {
  std::cerr << "\n\nfailWith: " << message << "\n\n";
  std::terminate();
}

[[noreturn]] void failWith(const std::string &message) {
  failWith(message.c_str());
}
}