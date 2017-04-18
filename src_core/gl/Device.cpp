#include <autograph/gl/Capture.h>
#include <autograph/gl/Device.h>
#include <autograph/gl/Fence.h>
#include <autograph/gl/Framebuffer.h>
#include <autograph/gl/UploadBuffer.h>
#include <autograph/support/Debug.h>
#include <cstring>
#include <iostream>
#include <memory>
#include <sstream>

namespace ag {
static GLImplementationLimits gGLImplementationLimits;

AG_GL_API const GLImplementationLimits &getGLImplementationLimits() {
  return gGLImplementationLimits;
}

namespace {
void APIENTRY debugCallback(gl::GLenum source, gl::GLenum type, gl::GLuint id,
	gl::GLenum severity, gl::GLsizei length, const gl::GLubyte *msg,
                            void *data) {
  if (severity != gl::DEBUG_SEVERITY_LOW &&
      severity != gl::DEBUG_SEVERITY_NOTIFICATION)
    AG_DEBUG("GL: {}", msg);
}

void setDebugCallback() {
  gl::Enable(gl::DEBUG_OUTPUT_SYNCHRONOUS);
  gl::DebugMessageCallback((gl::GLDEBUGPROC)debugCallback, nullptr);
  gl::DebugMessageControl(gl::DONT_CARE, gl::DONT_CARE, gl::DONT_CARE, 0,
                          nullptr, true);
  gl::DebugMessageInsert(gl::DEBUG_SOURCE_APPLICATION, gl::DEBUG_TYPE_MARKER,
                         1111, gl::DEBUG_SEVERITY_NOTIFICATION, -1,
                         "Started logging OpenGL messages");
}

uint64_t getFrameExpirationDate(uint64_t frame_id) {
  // Frame N expires when the fence has reached the value N+1
  return frame_id + 1;
}
} // namespace

DeviceConfig g_device_config;
Fence g_frame_fence;
uint64_t g_frame_id;
Framebuffer g_screen_fbo;
std::unique_ptr<UploadBuffer> g_default_upload_buffer;

static bool isInitialized() {
  return g_device_config.max_frames_in_flight != 0;
}

void pushDebugGroup(const char *message) {
  gl::PushDebugGroup(gl::DEBUG_SOURCE_APPLICATION, 0,
                     static_cast<gl::GLsizei>(std::strlen(message)), message);
}

void popDebugGroup() { gl::PopDebugGroup(); }

BufferSlice uploadFrameData(const void *data, size_t size, size_t alignment) {
  assert(isInitialized());
  AG_FRAME_TRACE("data={}, size={}, alignment={}", data, size, alignment);
  BufferSlice out_slice;
  if (alignment == -1) {
    alignment = gGLImplementationLimits.uniform_buffer_alignment;
  }
  if (!g_default_upload_buffer->upload(data, size, alignment,
                                       getFrameExpirationDate(g_frame_id),
                                       out_slice)) {
    throw std::runtime_error("Upload buffer is full");
  }
  return out_slice;
}

Framebuffer &getDefaultFramebuffer() {
  assert(isInitialized());
  return g_screen_fbo;
}

void endFrame() {
  assert(isInitialized());
  // sync on frame N-(max-in-flight)
  g_frame_id++;
  AG_FRAME_TRACE("g_frame_id={}", g_frame_id);
  g_frame_fence.signal(g_frame_id);
  if (g_frame_id >= g_device_config.max_frames_in_flight) {
    g_frame_fence.wait(getFrameExpirationDate(
        g_frame_id - g_device_config.max_frames_in_flight));
    g_default_upload_buffer->reclaim(getFrameExpirationDate(
        g_frame_id - g_device_config.max_frames_in_flight));
  }
}

void initialize(const DeviceConfig &config) {
  // glloadgen: load function pointers
  gl::sys::LoadFunctions();
  g_device_config = config;
  // g_screen_fbo.width = config.init_fb_width;
  // g_screen_fbo.height = config.init_fb_height;
  setDebugCallback();
  constexpr std::size_t upload_buf_size = 3 * 1024 * 1024;
  g_frame_fence = Fence{0};
  gl::GetIntegerv(gl::UNIFORM_BUFFER_OFFSET_ALIGNMENT,
                  &gGLImplementationLimits.uniform_buffer_alignment);
  g_default_upload_buffer = std::make_unique<UploadBuffer>(upload_buf_size);
}

void resizeDefaultFramebuffer(int w, int h) {
  assert(isInitialized());
  AG_FRAME_TRACE("g_frame_id={}", g_frame_id);
  g_screen_fbo = Framebuffer::createDefault(w, h);
}

uint64_t getFrameCount() { return g_frame_id; }
} // namespace ag
