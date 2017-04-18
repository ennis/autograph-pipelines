#include <autograph/gl/Capture.h>
#include <autograph/gl/Device.h>
#include <autograph/gl/StateGroup.h>

namespace ag {
static uint64_t nextFrameCapture = (uint64_t)-1;

void setFrameCapture(uint64_t frameID) { nextFrameCapture = frameID; }

void setNextFrameCapture() { nextFrameCapture = getFrameCount() + 1; }

bool isFrameCaptureRequested() {
  return nextFrameCapture == getFrameCount();
}
}