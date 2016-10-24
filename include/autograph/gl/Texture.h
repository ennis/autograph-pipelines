#pragma once
#include <autograph/ImageDesc.h>
#include <autograph/ImageFormat.h>
#include <autograph/Types.h>
#include <autograph/gl/GLHandle.h>

namespace ag {
namespace gl {

struct TextureDeleter {
  void operator()(GLuint tex_obj) { glDeleteTextures(1, &tex_obj); }
};

enum class TextureDimension {
  Texture1D,
  Texture2D,
  Texture3D,
  TextureCube,
  Texture2DArray
};

class AG_API Texture {
public:
  Texture() = default;
  Texture(const ImageDesc &desc);

  //====================================
  // Texture properties
  auto width() const { return desc_.width; }
  auto height() const { return desc_.height; }
  auto depth() const { return desc_.depth; }
  auto size(int mip_level = 0) const {
    return ivec3{desc_.width, desc_.height, desc_.depth};
  }
  auto format() const { return desc_.format; }
  void generateMipmaps();
  const auto &desc() const { return desc_; }
  GLuint object() const { return obj_.get(); }
  explicit operator bool() const { return obj_.get() != 0; }

  //====================================
  // Pixel transfer
  void upload(void *src, unsigned mipLevel = 0);
  void get(void *dest, unsigned mipLevel = 0);
  void getRegion(void *dest, unsigned x, unsigned y, unsigned width,
                 unsigned height, unsigned mipLevel = 0);
  vec4 texelFetch(ivec3 coords, unsigned mip_level = 0);

  //====================================
  // Constructors
  static Texture create1D(unsigned w, ImageFormat fmt, unsigned numMipmaps = 1);
  static Texture create2D(unsigned w, unsigned h, ImageFormat fmt,
                          unsigned numMipmaps = 1);

private:
  GLHandle<TextureDeleter> obj_;
  ImageDesc desc_;
};

struct GLFormatInfo {
  GLenum internal_fmt;
  GLenum external_fmt;
  GLenum type;
  int num_comp;
  int size;
};

const GLFormatInfo &getGLImageFormatInfo(ImageFormat fmt);
}
}