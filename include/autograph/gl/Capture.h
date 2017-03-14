#pragma once
#include <autograph/gl/Config.h>
#include <autograph/support/Debug.h>

namespace ag {
namespace gl {
// indicate that the given frame should be captured
AG_GL_API void setFrameCapture(uint64_t frameID);
// indicate that the next frame should be captured
AG_GL_API void setNextFrameCapture();
// is the current frame being captured
AG_GL_API bool isFrameCaptureRequested();

template <typename... Args>
void frameTrace(const char *function, const char *message, Args &&... args) {
  std::string str{function};
  str.append(": ");
  str.append(fmt::format(message, std::forward<Args>(args)...));
  rawLogMessage(LogLevel::Debug, str.c_str());
}

#ifdef AG_FRAME_TRACE_OUTPUT
#define AG_FRAME_TRACE(...)                                                    \
  do {                                                                         \
    if (ag::gl::isFrameCaptureRequested())                                     \
      ag::gl::frameTrace(__func__, __VA_ARGS__);                               \
  } while (0);
#else
#define AG_FRAME_TRACE(...)
#endif
}
}