#pragma once
#include <cstdint>
#include <deque>
#include <gl_core_4_5.hpp>
#include <stdexcept>

struct gl_fence {
  static constexpr unsigned fence_wait_timeout = 2000000000;

  struct sync_point {
    GLsync sync;
    uint64_t target_value;
  };

  gl_fence() = default;
  gl_fence(uint64_t init_value) : current_value(init_value) {}

  gl_fence(const gl_fence &) = delete;
  gl_fence &operator=(const gl_fence &) = delete;

  gl_fence(gl_fence &&rhs)
      : current_value(rhs.current_value),
        sync_points(std::move(rhs.sync_points)) {
    rhs.sync_points.clear();
  }
  gl_fence &operator=(gl_fence &&rhs) {
    std::swap(sync_points, rhs.sync_points);
    std::swap(current_value, rhs.current_value);
    return *this;
  }

  ~gl_fence() {
    for (auto s : sync_points)
      gl::DeleteSync(s.sync);
  }

  void signal(uint64_t value);
  GLenum advance(uint64_t timeout);
  void wait(uint64_t value);
  uint64_t get_value();

  uint64_t current_value;
  std::deque<sync_point> sync_points;
};
