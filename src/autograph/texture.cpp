#include "texture.hpp"
#include <cassert>
#include <stdexcept>

namespace ag {
static gl_format_info glfmt_rgba8_unorm{GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, 4,
                                        4};
static gl_format_info glfmt_r8_unorm{GL_R8, GL_RED, GL_UNSIGNED_BYTE, 1, 1};
static gl_format_info glfmt_r32_float{GL_R32F, GL_RED, GL_FLOAT, 1, 4};
static gl_format_info glfmt_rg32_float{GL_RG32F, GL_RG, GL_FLOAT, 2, 8};
static gl_format_info glfmt_rgba32_float{GL_RGBA32F, GL_RGBA, GL_FLOAT, 4, 16};
static gl_format_info glfmt_depth32_float{GL_DEPTH_COMPONENT32F,
                                          GL_DEPTH_COMPONENT, GL_FLOAT, 1, 4};

const gl_format_info &get_gl_image_format_info(image_format fmt) {
  switch (fmt) {
  case image_format::rgba8_unorm:
    return glfmt_rgba8_unorm;
  case image_format::r8_unorm:
    return glfmt_r8_unorm;
  case image_format::r32_float:
    return glfmt_r32_float;
  case image_format::rg32_float:
    return glfmt_rg32_float;
  case image_format::rgba32_float:
    return glfmt_rgba32_float;
  case image_format::depth32_float:
    return glfmt_depth32_float;
  default:
    throw std::logic_error("Unsupported image format");
  }
}

texture::texture(const image_desc &desc) : desc_{desc} {
  assert(desc.num_mips > 0);
  GLuint tex_obj;
  const auto &glfmt = get_gl_image_format_info(desc.format);
  switch (desc.dimensions) {
  case image_dimensions::img_1d:
    glCreateTextures(GL_TEXTURE_1D, 1, &tex_obj);
    glTextureStorage1D(tex_obj, desc.num_mips, glfmt.internal_fmt, desc.width);
    break;
  case image_dimensions::img_2d:
    glCreateTextures(GL_TEXTURE_2D, 1, &tex_obj);
    glTextureStorage2D(tex_obj, desc.num_mips, glfmt.internal_fmt, desc.width,
                       desc.height);
    break;
  case image_dimensions::img_3d:
    glCreateTextures(GL_TEXTURE_3D, 1, &tex_obj);
    glTextureStorage3D(tex_obj, desc.num_mips, glfmt.internal_fmt, desc.width,
                       desc.height, desc.depth);
    break;
  default:
    throw std::logic_error("Unsupported texture type");
  }
  // set sensible defaults
  glTextureParameteri(tex_obj, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTextureParameteri(tex_obj, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTextureParameteri(tex_obj, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  obj_ = gl_handle<texture_deleter>{tex_obj};
}

void texture::upload(void *src, unsigned mipLevel) {
  auto gl_fmt = get_gl_image_format_info(desc_.format);
  switch (desc_.dimensions) {
  case image_dimensions::img_1d:
    glTextureSubImage1D(obj_.get(), mipLevel, 0, desc_.width,
                        gl_fmt.external_fmt, gl_fmt.type, src);
  case image_dimensions::img_2d:
    glTextureSubImage2D(obj_.get(), mipLevel, 0, 0, desc_.width, desc_.height,
                        gl_fmt.external_fmt, gl_fmt.type, src);
  case image_dimensions::img_3d:
    glTextureSubImage3D(obj_.get(), mipLevel, 0, 0, 0, desc_.width,
                        desc_.height, desc_.depth, gl_fmt.external_fmt,
                        gl_fmt.type, src);
  }
}

void texture::get(void *dest, unsigned mipLevel) {
  auto gl_fmt = get_gl_image_format_info(desc_.format);
  glGetTextureImage(obj_.get(), mipLevel, gl_fmt.external_fmt, gl_fmt.type,
                    gl_fmt.size * desc_.width * desc_.height * desc_.depth,
                    dest);
}

void texture::generate_mipmaps() { glGenerateTextureMipmap(obj_.get()); }

void texture::get_region(void *dest, unsigned x, unsigned y, unsigned width,
                         unsigned height, unsigned mipLevel) {
  auto gl_fmt = get_gl_image_format_info(desc_.format);
  glGetTextureSubImage(obj_.get(), mipLevel, x, y, 0, width, height, 1,
                       gl_fmt.external_fmt, gl_fmt.type,
                       gl_fmt.size * width * height, dest);
}

glm::vec4 texture::texel_fetch(glm::ivec3 coords, unsigned mip_level) {
  glm::vec4 out;
  glGetTextureSubImage(obj_.get(), mip_level, coords.x, coords.y, coords.z, 1,
                       1, 1, GL_RGBA, GL_FLOAT, 4 * 4, &out);
  return out;
}

texture texture::create_1d(unsigned w, image_format fmt, unsigned num_mips) {
  image_desc d;
  d.dimensions = image_dimensions::img_1d;
  d.format = fmt;
  d.width = w;
  d.height = 1;
  d.depth = 1;
  d.num_mips = num_mips;
  return texture{d};
}

texture texture::create_2d(unsigned w, unsigned h, image_format fmt,
                           unsigned num_mips) {
  image_desc d;
  d.dimensions = image_dimensions::img_2d;
  d.format = fmt;
  d.width = w;
  d.height = h;
  d.depth = 1;
  d.num_mips = num_mips;
  return texture{d};
}
}