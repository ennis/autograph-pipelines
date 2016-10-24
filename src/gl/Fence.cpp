#include <autograph/gl/Fence.h>

namespace ag {
namespace gl {

void Fence::signal(uint64_t value) {
  auto sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
  sync_points.push_back(sync_point{sync, value});
}

GLenum Fence::advance(uint64_t timeout) {
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

void Fence::wait(uint64_t value) {
  while (getValue() < value) {
    auto waitResult = advance(kFenceWaitTimeout);
    if (waitResult == GL_TIMEOUT_EXPIRED)
      throw std::runtime_error{"Timeout expired while waiting for fence"};
  }
}

uint64_t Fence::getValue() {
  while (!sync_points.empty()) {
    auto waitResult = advance(0);
    if (waitResult == GL_TIMEOUT_EXPIRED)
      break;
  }
  return current_value;
}
}
}