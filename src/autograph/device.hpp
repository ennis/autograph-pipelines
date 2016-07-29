#ifndef DEVICE_HPP
#define DEVICE_HPP
#include <array>
#include "buffer.hpp"
#include "framebuffer.hpp"

namespace ag {
struct device_config {
  unsigned init_fb_width;
  unsigned init_fb_height;
  unsigned max_frames_in_flight;
};

struct gl_impl_limits {
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

extern gl_impl_limits g_gl_impl_limits;
inline const gl_impl_limits &get_impl_limits() {
	return g_gl_impl_limits;
}

framebuffer& get_default_framebuffer();
void initialize(const device_config &cfg);
// resize the framebuffer 0 of the current context
void resize_default_framebuffer(glm::ivec2 size);
buffer_slice upload_frame_data(const void *data, size_t size, size_t alignment);
template <typename T>
buffer_slice upload_frame_array(const T *data, size_t num_elements,
                                size_t alignment = alignof(T)) {
  return upload_frame_data(&data, num_elements * sizeof(T), alignment);
}

template <typename T, size_t N>
buffer_slice upload_frame_array(const std::array<T, N> &data,
                                size_t alignment = alignof(T)) {
  return upload_frame_data(data.data(), N * sizeof(T), alignment);
}

uint64_t get_frame_count();
void end_frame();
}

#endif
