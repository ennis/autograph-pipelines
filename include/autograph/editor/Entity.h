#pragma once
#include <autograph/Config.h>
#include <cstdint>

namespace ag {
struct  Entity {
  uint64_t id;

  operator uint64_t() { return id; }

  int getIndex() { return static_cast<int>(id & 0xFFFFFFFF); }

  int getGeneration() { return static_cast<int>(id >> 32); }
};
}