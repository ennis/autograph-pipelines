#include "device.hpp"
#include "framebuffer.hpp"
#include "upload_buffer.hpp"
#include "fence.hpp"
#include <cstring>
#include <iostream>
#include <sstream>
#include <memory>

namespace ag {
	gl_impl_limits g_gl_impl_limits;

namespace {
void APIENTRY debugCallback(GLenum source, GLenum type, GLuint id,
                            GLenum severity, GLsizei length, const GLubyte *msg,
                            void *data) {
  if (severity != GL_DEBUG_SEVERITY_LOW &&
      severity != GL_DEBUG_SEVERITY_NOTIFICATION)
    std::clog << "(GL) " << msg << std::endl;
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

device_config g_device_config;
fence g_frame_fence;
uint64_t g_frame_id;
framebuffer g_screen_fbo;
std::unique_ptr<upload_buffer> g_default_upload_buffer;

bool is_initialized()
{
	return g_device_config.max_frames_in_flight != 0;
}

void push_debug_group(const char *message) {
  glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0,
                   static_cast<GLsizei>(std::strlen(message)), message);
}

void pop_debug_group() { glPopDebugGroup(); }

buffer_slice upload_frame_data(const void *data, size_t size,
                                  size_t alignment) 
{
	assert(is_initialized());
  buffer_slice out_slice;
  if (!g_default_upload_buffer->upload(data, size, alignment,
                                       getFrameExpirationDate(g_frame_id),
                                       out_slice)) {
    throw std::runtime_error("Upload buffer is full");
  }
  return out_slice;
}

framebuffer& get_default_framebuffer()
{
	assert(is_initialized());
    return g_screen_fbo;
}

void end_frame() {
	assert(is_initialized());
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

void initialize(const device_config &config) {
	// glloadgen: load function pointers
	ogl_LoadFunctions();
  g_device_config = config;
  //g_screen_fbo.width = config.init_fb_width;
  //g_screen_fbo.height = config.init_fb_height;
  setDebugCallback();
  constexpr std::size_t upload_buf_size = 3 * 1024 * 1024;
  g_frame_fence = fence{0};
  g_default_upload_buffer = std::make_unique<upload_buffer>(upload_buf_size);
}

void resize_screen(glm::ivec2 size)
{
	assert(is_initialized());
    g_screen_fbo = framebuffer::create_default(size);
}

uint64_t get_frame_count() { return g_frame_id; }
}
