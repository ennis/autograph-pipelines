#include "fence.hpp"

namespace ag {

void fence::signal(uint64_t value) {
  auto sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
  sync_points.push_back(sync_point{sync, value});
}

GLenum fence::advance(uint64_t timeout) {
  auto &targetSyncPoint = sync_points.front();
  auto waitResult = glClientWaitSync(targetSyncPoint.sync,
                                     GL_SYNC_FLUSH_COMMANDS_BIT, timeout);
  if (waitResult == GL_CONDITION_SATISFIED ||
      waitResult == GL_ALREADY_SIGNALED) {
    current_value = targetSyncPoint.target_value;
    glDeleteSync(targetSyncPoint.sync);
    sync_points.pop_front();
  } else if (waitResult == GL_WAIT_FAILED)
    throw std::runtime_error{"Wait failed while waiting for fence"};
  return waitResult;
}

void fence::wait(uint64_t value) {
  while (get_value() < value) {
    auto waitResult = advance(fence_wait_timeout);
    if (waitResult == GL_TIMEOUT_EXPIRED)
      throw std::runtime_error{"Timeout expired while waiting for fence"};
  }
}

uint64_t fence::get_value() {
  while (!sync_points.empty()) {
    auto waitResult = advance(0);
    if (waitResult == GL_TIMEOUT_EXPIRED)
      break;
  }
  return current_value;
}
}