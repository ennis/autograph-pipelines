#include <autograph/Gfx/Fence.h>
#include <autograph/Gfx/GfxContext.h>

namespace ag {

void Fence::signal(uint64_t value) {
  AG_FRAME_TRACE("this={}, value={}", (const void*)this, value);
  auto sync = gl::FenceSync(gl::SYNC_GPU_COMMANDS_COMPLETE, 0);
  sync_points.push_back(sync_point{sync, value});
}

gl::GLenum Fence::advance(uint64_t timeout) {
  auto &targetSyncPoint = sync_points.front();
  auto waitResult = gl::ClientWaitSync(targetSyncPoint.sync,
                                     gl::SYNC_FLUSH_COMMANDS_BIT, timeout);
  if (waitResult == gl::CONDITION_SATISFIED ||
      waitResult == gl::ALREADY_SIGNALED) {
    current_value = targetSyncPoint.target_value;
	AG_FRAME_TRACE("this={}, current_value={}", (const void*)this, current_value);
    gl::DeleteSync(targetSyncPoint.sync);
    sync_points.pop_front();
  } else if (waitResult == gl::WAIT_FAILED_)
    throw std::runtime_error{"Wait failed while waiting for fence"};
  return waitResult;
}

void Fence::wait(uint64_t value) {
  AG_FRAME_TRACE("this={}, value={}", (const void*)this, value);
  while (getValue() < value) {
    auto waitResult = advance(kFenceWaitTimeout);
    if (waitResult == gl::TIMEOUT_EXPIRED)
      throw std::runtime_error{"Timeout expired while waiting for fence"};
  }
}

uint64_t Fence::getValue() {
  while (!sync_points.empty()) {
    auto waitResult = advance(0);
    if (waitResult == gl::TIMEOUT_EXPIRED)
      break;
  }
  return current_value;
}
}
