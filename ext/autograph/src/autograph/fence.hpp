#ifndef FENCE_HPP
#define FENCE_HPP

#include <cstdint>
#include <deque>
#include "gl_types.hpp"

namespace ag {
using FenceValue = uint64_t;

struct Fence {
  static constexpr unsigned kFenceWaitTimeout = 2000000000; // in nanoseconds

  struct SyncPoint {
    GLsync sync;
    uint64_t target_value;
  };

  Fence() = default;
  Fence(const Fence&) = delete;
  Fence& operator=(const Fence&) = delete;
  Fence(Fence&& rhs)
      : current_value(rhs.current_value),
        sync_points(std::move(rhs.sync_points)) {
    rhs.sync_points.clear();
  }
  Fence& operator=(Fence&& rhs) {
	  std::swap(sync_points, rhs.sync_points);
	  std::swap(current_value, rhs.current_value);
	  return *this;
  }

  ~Fence() {
    for (auto s : sync_points)
      gl::DeleteSync(s.sync);
  }

  void signal(uint64_t value) {
    auto sync = gl::FenceSync(gl::SYNC_GPU_COMMANDS_COMPLETE, 0);
    sync_points.push_back(Fence::SyncPoint{sync, value});
  }

  GLenum advance(uint64_t timeout) {
    auto& targetSyncPoint = sync_points.front();
    auto waitResult = gl::ClientWaitSync(targetSyncPoint.sync,
                                         gl::SYNC_FLUSH_COMMANDS_BIT, timeout);
    if (waitResult == gl::CONDITION_SATISFIED ||
        waitResult == gl::ALREADY_SIGNALED) {
      current_value = targetSyncPoint.target_value;
      gl::DeleteSync(targetSyncPoint.sync);
      sync_points.pop_front();
    } else if (waitResult == gl::WAIT_FAILED_)
      failWith("Wait failed while waiting for fence");
    return waitResult;
  }

  void wait(uint64_t value) {
    while (get_value() < value) {
      auto waitResult = advance(kFenceWaitTimeout);
      if (waitResult == gl::TIMEOUT_EXPIRED)
        failWith("Timeout expired while waiting for fence");
    }
  }

  uint64_t get_value() {
    while (!sync_points.empty()) {
      auto waitResult = advance(0);
      if (waitResult == gl::TIMEOUT_EXPIRED)
        break;
    }
    return current_value;
  }

  uint64_t current_value;
  std::deque<SyncPoint> sync_points;
};
}

#endif // !FENCE_HPP
