#pragma once
#include <memory>
#include "gl_fence.hpp"
#include "gl_framebuffer.hpp"
#include "upload_buffer.hpp"

struct gl_device_config {
  unsigned init_fb_width;
  unsigned init_fb_height;
  unsigned max_frames_in_flight;
};

// shared global stuff
struct gl_device {
  gl_device(const gl_device_config &config_);
  ~gl_device() {}

  static void init(const gl_device_config &config);
  static void set_size(unsigned width, unsigned height);
  void end_frame();

  gl_buffer_slice upload_frame_data(const void *data, size_t size,
                                    size_t alignment);

  gl_device_config config;
  gl_fence frame_fence;
  unsigned frame_id;
  gl_framebuffer screen_fbo;
  std::unique_ptr<upload_buffer> default_upload_buffer;
};

struct debug_group
{
	debug_group(std::string msg);
	 ~debug_group();
};

extern std::unique_ptr<gl_device> g_device;
