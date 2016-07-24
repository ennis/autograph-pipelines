#ifndef GL_FENCE_HPP
#define GL_FENCE_HPP
#include <cstdint>
#include <deque>
#include <gl_core_4_5.h>
#include <stdexcept>

namespace ag {
constexpr unsigned fence_wait_timeout = 2000000000; // in nanoseconds

class fence {
public:

  fence() = default;
  fence(uint64_t init_value) : current_value(init_value) {}

  fence(const fence &) = delete;
  fence &operator=(const fence &) = delete;

  fence(fence &&rhs)
      : current_value(rhs.current_value),
        sync_points(std::move(rhs.sync_points)) {
    rhs.sync_points.clear();
  }
  fence &operator=(fence &&rhs) {
    std::swap(sync_points, rhs.sync_points);
    std::swap(current_value, rhs.current_value);
    return *this;
  }

  ~fence() {
    for (auto s : sync_points)
      glDeleteSync(s.sync);
  }

  void signal(uint64_t value);
  GLenum advance(uint64_t timeout);
  void wait(uint64_t value);
  uint64_t get_value();

private:
  struct sync_point {
    GLsync sync;
    uint64_t target_value;
  };
  
  uint64_t current_value;
  std::deque<sync_point> sync_points;
};
}

#endif // !gl_fence_HPP
