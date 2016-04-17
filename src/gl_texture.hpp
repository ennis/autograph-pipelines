#pragma once
#include "format.hpp"
#include "gl_handle.hpp"
#include "image_desc.hpp"
#include <gl_core_4_5.hpp>

struct texture_deleter {
  void operator()(GLuint tex_obj) { gl::DeleteTextures(1, &tex_obj); }
};

enum class texture_dimension { tex_1d, tex_2d, tex_3d };

struct gl_texture {
	gl_texture(const image_desc& desc);

  gl_handle<texture_deleter> obj_;
  image_desc desc_;
};

struct gl_format_info {
  GLenum internal_fmt;
  GLenum external_fmt;
  GLenum type;
  int num_comp;
};

const gl_format_info &get_gl_image_format_info(image_format fmt);
