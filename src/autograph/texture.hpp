#pragma once
#include "format.hpp"
#include "gl_handle.hpp"
#include "image_desc.hpp"
#include <glm/glm.hpp>

namespace ag {

struct texture_deleter {
  void operator()(GLuint tex_obj) { glDeleteTextures(1, &tex_obj); }
};

enum class texture_dimension { tex_1d, tex_2d, tex_3d, tex_cube, tex_2d_array };

class texture {
public:
  texture() = default;
  texture(const image_desc &desc);

  //====================================
  // Texture properties
  auto width() const { return desc_.width; }
  auto height() const { return desc_.height; }
  auto depth() const { return desc_.depth; }
  auto size(int mip_level = 0) const {
	  return glm::ivec3{ desc_.width, desc_.height, desc_.depth };
  }
  auto format() const { return desc_.format; }
  void generate_mipmaps();
  const auto &desc() const { return desc_; }
  GLuint object() const { return obj_.get(); }
  explicit operator bool() const { return obj_.get() != 0; }

  //====================================
  // Pixel transfer
  void upload(void *src, unsigned mipLevel = 0);
  void get(void *dest, unsigned mipLevel = 0);
  void get_region(void *dest, unsigned x, unsigned y, unsigned width,
                  unsigned height, unsigned mipLevel = 0);
  glm::vec4 texel_fetch(glm::ivec3 coords, unsigned mip_level = 0);

  //====================================
  // Constructors
  static texture create_1d(unsigned w, image_format fmt, unsigned num_mips = 1);
  static texture create_2d(unsigned w, unsigned h, image_format fmt,
                           unsigned num_mips = 1);

private:
  gl_handle<texture_deleter> obj_;
  image_desc desc_;
};

struct gl_format_info {
  GLenum internal_fmt;
  GLenum external_fmt;
  GLenum type;
  int num_comp;
  int size;
};

const gl_format_info &get_gl_image_format_info(image_format fmt);
}
