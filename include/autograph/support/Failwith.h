#pragma once
#include <string>

namespace ag {
// Fail with a message and call std::terminate
[[noreturn]] void failWith(const char *message);
[[noreturn]] void failWith(const std::string &message);
}