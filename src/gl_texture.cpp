#include "gl_texture.hpp"
#include <stdexcept>

static gl_format_info glfmt_rgba8_unorm{gl::RGBA8, gl::RGBA, gl::UNSIGNED_BYTE,
                                        4};
static gl_format_info glfmt_r32_float{gl::R32F, gl::RED, gl::FLOAT, 1};
static gl_format_info glfmt_rg32_float{gl::RG32F, gl::RG, gl::FLOAT, 2};
static gl_format_info glfmt_rgba32_float{gl::RGBA32F, gl::RGBA, gl::FLOAT, 4};

const gl_format_info &get_gl_image_format_info(image_format fmt) {
  switch (fmt) {
  case image_format::rgba8_unorm:
    return glfmt_rgba8_unorm;
  case image_format::r32_float:
    return glfmt_r32_float;
  case image_format::rg32_float:
    return glfmt_rg32_float;
  case image_format::rgba32_float:
    return glfmt_rgba32_float;
  default:
    throw std::logic_error("Unsupported image format");
  }
}

gl_texture::gl_texture(const image_desc &desc) : desc_{desc} {
  assert(desc.num_mips > 0);
  GLuint tex_obj;
  const auto& glfmt = get_gl_image_format_info(desc.format);
  switch (desc.dimensions) {
  case image_dimensions::img_1d:
    gl::CreateTextures(gl::TEXTURE_1D, desc.num_mips, &tex_obj);
    gl::TextureStorage1D(tex_obj, 1, glfmt.internal_fmt, desc.width);
    break;
  case image_dimensions::img_2d:
    gl::CreateTextures(gl::TEXTURE_2D, desc.num_mips, &tex_obj);
    gl::TextureStorage2D(tex_obj, 1, glfmt.internal_fmt, desc.width,
                         desc.height);
    break;
  case image_dimensions::img_3d:
    gl::CreateTextures(gl::TEXTURE_3D, desc.num_mips, &tex_obj);
    gl::TextureStorage3D(tex_obj, 1, glfmt.internal_fmt, desc.width,
                         desc.height, desc.depth);
    break;
  default:
    throw std::logic_error("Unsupported texture type");
  }
  obj_ = gl_handle<texture_deleter>{tex_obj};
}
