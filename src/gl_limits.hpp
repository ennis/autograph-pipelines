#pragma once

struct gl_impl_limits
{
	int max_vertex_attributes;
	int max_vertex_buffers;
	int max_texture_units;
	int max_draw_buffers;
	int max_3d_texture_size_w;
	int max_3d_texture_size_h;
	int max_3d_texture_size_d;
	int max_combined_texture_image_units;
	int max_combined_uniform_blocks;
	int max_combined_shader_storage_blocks;
	int max_compute_texture_image_units;
	int max_compute_uniform_blocks;
	int max_compute_shader_storage_blocks;
	int max_compute_work_group_invocations;
	int max_compute_work_group_count;
	int max_compute_work_group_size;

	int uniform_buffer_alignment;

	int default_compute_local_size_x;
	int default_compute_local_size_y;
	int default_compute_local_size_z;
};

extern gl_impl_limits g_gl_impl_limits;