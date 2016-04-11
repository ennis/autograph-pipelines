#ifndef ERROR_HPP
#define ERROR_HPP

#include <string>
#include <stdexcept>
#include <iostream>

namespace ag {
[[noreturn]] inline void failWith(std::string message) {
  std::cerr << "ABORT: " << message << "\n";
  throw std::runtime_error(message.c_str());
}
}

#endif // !ERROR_HPP
