#include <autograph/gl/Capture.h>
#include <autograph/gl/Device.h>
#include <autograph/gl/StateGroup.h>

namespace ag {
namespace gl {
static uint64_t nextFrameCapture = (uint64_t)-1;

AG_API void setFrameCapture(uint64_t frameID) { nextFrameCapture = frameID; }

AG_API void setNextFrameCapture() { nextFrameCapture = getFrameCount() + 1; }

AG_API bool isFrameCaptureRequested() {
  return nextFrameCapture == getFrameCount();
}
}
}