#pragma once
#include <autograph/Engine/Event.h>
#include <autograph/Gfx/Framebuffer.h>
#include <functional>

namespace ag {

// == Presentation surface + inputs
class Window {
public:
  virtual ivec2 getFramebufferSize() = 0;
  virtual ivec2 getWindowSize() = 0;
  virtual ivec2 getCursorPos() = 0;
  virtual void unsetEventCallback() = 0;
  virtual void
  setEventCallback(std::function<void(const Event &, Window *)> callback) = 0;
  virtual void swapBuffers() = 0;
  virtual void prepareForRender() = 0;
  // virtual Framebuffer &getFramebuffer() = 0;
  virtual bool isFocused() = 0;

private:
};
} // namespace ag