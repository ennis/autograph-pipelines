#pragma once
#include <array>
#include <autograph/Types.h>
#include <autograph/gl/Buffer.h>
#include <autograph/gl/Framebuffer.h>

namespace ag {
namespace gl {
struct DeviceConfig {
  unsigned init_fb_width = 0;
  unsigned init_fb_height = 0;
  unsigned max_frames_in_flight = 0;
};

struct GLImplementationLimits {
  int max_vertex_attributes = 16;
  int max_vertex_buffers = 16;
  int max_texture_units = 16;
  int max_draw_buffers = 16;
  int max_3d_texture_size_w;
  int max_3d_texture_size_h;
  int max_3d_texture_size_d;
  int max_combined_texture_image_units = 16;
  int max_combined_uniform_blocks = 16;
  int max_combined_shader_storage_blocks = 16;
  int max_compute_texture_image_units = 16;
  int max_compute_uniform_blocks = 16;
  int max_compute_shader_storage_blocks = 16;
  int max_compute_work_group_invocations;
  int max_compute_work_group_count;
  int max_compute_work_group_size;
  int uniform_buffer_alignment = 256;
  int default_compute_local_size_x;
  int default_compute_local_size_y;
  int default_compute_local_size_z;
};

extern GLImplementationLimits gGLImplementationLimits;
AG_API inline const GLImplementationLimits &getGLImplementationLimits() {
  return gGLImplementationLimits;
}

AG_API Framebuffer &getDefaultFramebuffer();
AG_API void initialize(const DeviceConfig &cfg);
// resize the framebuffer 0 of the current context
AG_API void resizeDefaultFramebuffer(int w, int h);
AG_API BufferSlice uploadFrameData(const void *data, size_t size,
                                   size_t alignment = -1);

template <typename T>
inline BufferSlice uploadFrameArray(const T *data, size_t num_elements,
                                    size_t alignment = -1) {
  return uploadFrameData(data, num_elements * sizeof(T), alignment);
}

template <typename T, size_t N>
inline BufferSlice uploadFrameArray(const std::array<T, N> &data,
                                    size_t alignment = -1) {
  return uploadFrameData(data.data(), N * sizeof(T), alignment);
}

template <typename T, size_t N>
inline BufferSlice uploadFrameArray(T (&data)[N], size_t alignment = -1) {
  return uploadFrameData(&data[0], N * sizeof(T), alignment);
}

AG_API uint64_t getFrameCount();
AG_API void endFrame();
}
}