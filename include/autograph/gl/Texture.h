#pragma once
#include <autograph/ImageDesc.h>
#include <autograph/ImageFormat.h>
#include <autograph/Types.h>
#include <autograph/gl/GLHandle.h>

namespace ag {
namespace gl {

struct TextureDeleter {
	void operator()(GLuint tex_obj);
};

class AG_API Texture {
public:
  Texture() = default;

  Texture(Texture&& rhs) = default;
  Texture& operator=(Texture&& rhs) = default;

  ~Texture();

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
  void reset();

  //====================================
  // Pixel transfer
  void upload(void *src, int mipLevel = 0);
  void get(void *dest, int mipLevel = 0);
  void getRegion(void *dest, int x, int y, int width,
	  int height, int mipLevel = 0);
  vec4 texelFetch(ivec3 coords, int mip_level = 0);

  struct MipMaps { int count; };
  struct Samples { int count; };

  //====================================
  // Constructors
  static Texture Create1D(ImageFormat fmt, int w, MipMaps mipMaps = MipMaps{ 1 });
  static Texture Create2D(ImageFormat fmt, int w, int h, MipMaps mipMaps = MipMaps{ 1 });
  static Texture Create2DMultisample(ImageFormat fmt, int w, int h, Samples ms = Samples{ 0 });
  static Texture Create3D(ImageFormat fmt, int w, int h, int d, MipMaps mipMaps = MipMaps{ 1 });

private:
  GLHandle<TextureDeleter> obj_;
  int numSamples_{0};
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
