#include "gl_fence.hpp"

void gl_fence::signal(uint64_t value) {
  auto sync = gl::FenceSync(gl::SYNC_GPU_COMMANDS_COMPLETE, 0);
  sync_points.push_back(gl_fence::sync_point{sync, value});
}

GLenum gl_fence::advance(uint64_t timeout) {
  auto &targetSyncPoint = sync_points.front();
  auto waitResult = gl::ClientWaitSync(targetSyncPoint.sync,
                                       gl::SYNC_FLUSH_COMMANDS_BIT, timeout);
  if (waitResult == gl::CONDITION_SATISFIED ||
      waitResult == gl::ALREADY_SIGNALED) {
    current_value = targetSyncPoint.target_value;
    gl::DeleteSync(targetSyncPoint.sync);
    sync_points.pop_front();
  } else if (waitResult == gl::WAIT_FAILED_)
    throw std::runtime_error{"Wait failed while waiting for fence"};
  return waitResult;
}

void gl_fence::wait(uint64_t value) {
  while (get_value() < value) {
    auto waitResult = advance(fence_wait_timeout);
    if (waitResult == gl::TIMEOUT_EXPIRED)
      throw std::runtime_error{"Timeout expired while waiting for fence"};
  }
}

uint64_t gl_fence::get_value() {
  while (!sync_points.empty()) {
    auto waitResult = advance(0);
    if (waitResult == gl::TIMEOUT_EXPIRED)
      break;
  }
  return current_value;
}