#include "gl_device.hpp"
#include "gl_limits.hpp"
#include <sstream>
#include <iostream>
#include <cstring>

std::unique_ptr<gl_device> g_device;

namespace {
void APIENTRY debugCallback(GLenum source, GLenum type, GLuint id,
                            GLenum severity, GLsizei length, const GLubyte *msg,
                            void *data) {
  if (severity != gl::DEBUG_SEVERITY_LOW &&
      severity != gl::DEBUG_SEVERITY_NOTIFICATION)
    std::clog << "(GL) " << msg << std::endl;
}

void setDebugCallback() {
  gl::Enable(gl::DEBUG_OUTPUT_SYNCHRONOUS);
  gl::DebugMessageCallback((GLDEBUGPROC)debugCallback, nullptr);
  gl::DebugMessageControl(gl::DONT_CARE, gl::DONT_CARE, gl::DONT_CARE, 0,
                          nullptr, true);
  gl::DebugMessageInsert(gl::DEBUG_SOURCE_APPLICATION, gl::DEBUG_TYPE_MARKER,
                         1111, gl::DEBUG_SEVERITY_NOTIFICATION, -1,
                         "Started logging OpenGL messages");
}

uint64_t getFrameExpirationDate(unsigned frame_id) {
  // Frame N expires when the fence has reached the value N+1
  return frame_id + 1;
}
}

// constructor
gl_device::gl_device(const gl_device_config &config_)
    : config{config_}, frame_id{0}, screen_fbo{0} 
{
  screen_fbo.size_.x = config_.init_fb_width;
  screen_fbo.size_.y = config_.init_fb_height;
  setDebugCallback();
  constexpr std::size_t upload_buf_size = 3 * 1024 * 1024;
  frame_fence = gl_fence{0};
  default_upload_buffer = std::make_unique<upload_buffer>(upload_buf_size);

  // query implementation limits
  /*gl::GetIntegerv(&g_gl_impl_limits.uniform_buffer_alignment);
  gl::GetIntegerv();
  gl::GetIntegerv();
  gl::GetIntegerv();
  gl::GetIntegerv();
  gl::GetIntegerv();
  gl::GetIntegerv();*/
}

gl_buffer_slice gl_device::upload_frame_data(const void *data, size_t size,
                                             size_t alignment) {
  gl_buffer_slice out_slice;
  if (!default_upload_buffer->upload(
          data, size, alignment, getFrameExpirationDate(frame_id), out_slice)) {
    throw std::runtime_error("Upload buffer is full");
  }
  return std::move(out_slice);
}

void gl_device::end_frame() {
  // sync on frame N-(max-in-flight)
  frame_id++;
  frame_fence.signal(frame_id);
  if (frame_id >= config.max_frames_in_flight) {
    frame_fence.wait(
        getFrameExpirationDate(frame_id - config.max_frames_in_flight));
    default_upload_buffer->reclaim(
        getFrameExpirationDate(frame_id - config.max_frames_in_flight));
  }
}

void gl_device::init(const gl_device_config &config) {
  g_device = std::make_unique<gl_device>(config);
}

void gl_device::set_size(unsigned width, unsigned height)
{
	g_device->screen_fbo.size_ = glm::ivec2{ width, height };
}

debug_group::debug_group(std::string msg)
{
	gl::PushDebugGroup(gl::DEBUG_SOURCE_APPLICATION, 0, (GLsizei)msg.size(), msg.c_str());
}

debug_group::~debug_group()
{
	gl::PopDebugGroup();
}

