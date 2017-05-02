#include <autograph/Gfx/GfxContext.h>
#include <autograph/Gfx/Fence.h>
#include <autograph/Gfx/Framebuffer.h>
#include <autograph/Gfx/UploadBuffer.h>
#include <autograph/Core/Support/Debug.h>
#include <cstring>
#include <iostream>
#include <memory>
#include <sstream>

namespace ag {

static void APIENTRY debugCallback(gl::GLenum source, gl::GLenum type, gl::GLuint id,
                            gl::GLenum severity, gl::GLsizei length,
                            const gl::GLubyte *msg, void *data) {
  if (severity != gl::DEBUG_SEVERITY_LOW &&
      severity != gl::DEBUG_SEVERITY_NOTIFICATION)
    AG_DEBUG("GL: {}", msg);
}


static void setDebugCallback() {
  gl::Enable(gl::DEBUG_OUTPUT_SYNCHRONOUS);
  gl::DebugMessageCallback((gl::GLDEBUGPROC)debugCallback, nullptr);
  gl::DebugMessageControl(gl::DONT_CARE, gl::DONT_CARE, gl::DONT_CARE, 0,
                          nullptr, true);
  gl::DebugMessageInsert(gl::DEBUG_SOURCE_APPLICATION, gl::DEBUG_TYPE_MARKER,
                         1111, gl::DEBUG_SEVERITY_NOTIFICATION, -1,
                         "Started logging OpenGL messages");
}

static void pushDebugGroup(const char *message) {
  gl::PushDebugGroup(gl::DEBUG_SOURCE_APPLICATION, 0,
                     static_cast<gl::GLsizei>(std::strlen(message)), message);
}

static void popDebugGroup() { gl::PopDebugGroup(); }

const GLImplementationLimits &GfxContext::getGLImplementationLimits() {
  return implLimits;
}

/*Framebuffer &GfxContext::getDefaultFramebuffer() {
  return screenFbo;
}*/

void GfxContext::beginFrame() 
{
  // Nothing to do
}

void GfxContext::endFrame() {
  // sync on frame N-(max-in-flight)
  frameIndex++;
  AG_FRAME_TRACE("frameIndex={}", frameIndex);
  frameFence.signal(frameIndex);
  if (frameIndex >= cfg.maxFramesInFlight) {
    frameFence.wait(frameIndex - cfg.maxFramesInFlight + 1);
  }
}

GfxContext::GfxContext(const GfxConfig &config) {
  cfg = config;
  setDebugCallback();
  frameFence = Fence{0};
  // TODO query all implementation limits
  gl::GetIntegerv(gl::UNIFORM_BUFFER_OFFSET_ALIGNMENT,
                  &implLimits.uniform_buffer_alignment);
  // automatically set this instance as the current graphics context
  // this is relatively safe since the GfxContext object cannot be moved 
  // (the copy and move constructors have been disabled)
  setGfxContext(this);
}

GfxContext::~GfxContext()
{
  // unregister ourselves as the current global context if needed
  if (&getGfxContext() == this) {
    setGfxContext(nullptr);
  }
}

void GfxContext::resizeRenderWindow(int w, int h) {
  width = w;
  height = h;
  screenFbo = Framebuffer::createDefault(w, h);
}

int64_t GfxContext::getCurrentFrameIndex() { return frameIndex; }

//-------------------------------------------------------
// GfxContext instanace
static GfxContext* gGfxContext;

void setGfxContext(GfxContext* ctx)
{
  gGfxContext = ctx;
}

GfxContext &getGfxContext()
{
  assert(gGfxContext);
  return *gGfxContext;
}

} // namespace ag
