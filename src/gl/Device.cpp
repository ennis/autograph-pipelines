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
namespace gl {
GLImplementationLimits gGLImplementationLimits;

namespace {
void APIENTRY debugCallback(GLenum source, GLenum type, GLuint id,
                            GLenum severity, GLsizei length, const GLubyte *msg,
                            void *data) {
  //if (severity != GL_DEBUG_SEVERITY_LOW &&
     // severity != GL_DEBUG_SEVERITY_NOTIFICATION)
      AG_DEBUG("GL: {}", msg);
}

void setDebugCallback() {
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback((GLDEBUGPROC)debugCallback, nullptr);
  glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr,
                        true);
  glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 1111,
                       GL_DEBUG_SEVERITY_NOTIFICATION, -1,
                       "Started logging OpenGL messages");
}

uint64_t getFrameExpirationDate(uint64_t frame_id) {
  // Frame N expires when the fence has reached the value N+1
  return frame_id + 1;
}
}

DeviceConfig g_device_config;
Fence g_frame_fence;
uint64_t g_frame_id;
Framebuffer g_screen_fbo;
std::unique_ptr<UploadBuffer> g_default_upload_buffer;

static bool isInitialized() { return g_device_config.max_frames_in_flight != 0; }

AG_API void pushDebugGroup(const char *message) {
  glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0,
                   static_cast<GLsizei>(std::strlen(message)), message);
}

AG_API void popDebugGroup() { glPopDebugGroup(); }

AG_API BufferSlice uploadFrameData(const void *data, size_t size, size_t alignment) {
  assert(isInitialized());
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

AG_API Framebuffer &getDefaultFramebuffer() {
  assert(isInitialized());
  return g_screen_fbo;
}

AG_API void endFrame() {
  assert(isInitialized());
  // sync on frame N-(max-in-flight)
  g_frame_id++;
  g_frame_fence.signal(g_frame_id);
  if (g_frame_id >= g_device_config.max_frames_in_flight) {
    g_frame_fence.wait(getFrameExpirationDate(
        g_frame_id - g_device_config.max_frames_in_flight));
    g_default_upload_buffer->reclaim(getFrameExpirationDate(
        g_frame_id - g_device_config.max_frames_in_flight));
  }
}

AG_API void initialize(const DeviceConfig &config) {
  // glloadgen: load function pointers
  ogl_LoadFunctions();
  g_device_config = config;
  // g_screen_fbo.width = config.init_fb_width;
  // g_screen_fbo.height = config.init_fb_height;
  setDebugCallback();
  constexpr std::size_t upload_buf_size = 3*1024*1024 ;
  g_frame_fence = Fence{0};
  glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &gGLImplementationLimits.uniform_buffer_alignment);
  g_default_upload_buffer = std::make_unique<UploadBuffer>(upload_buf_size);
}

AG_API void resizeDefaultFramebuffer(int w, int h) {
  assert(isInitialized());
  g_screen_fbo = Framebuffer::createDefault(w, h);
}

AG_API uint64_t getFrameCount() { return g_frame_id; }
}
}
