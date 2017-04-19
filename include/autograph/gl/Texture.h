#pragma once
#include <autograph/ImageDesc.h>
#include <autograph/ImageFormat.h>
#include <autograph/Types.h>
#include <autograph/gl/GLHandle.h>

namespace ag {

struct AG_GL_API TextureDeleter {
	static constexpr gl::GLenum objectType = gl::TEXTURE;
	void operator()(gl::GLuint tex_obj);
};

class AG_GL_API Texture {
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
  gl::GLuint object() const { return obj_.get(); }
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
  static Texture create1D(ImageFormat fmt, int w, MipMaps mipMaps = MipMaps{ 1 });
  static Texture create2D(ImageFormat fmt, int w, int h, MipMaps mipMaps = MipMaps{ 1 });
  static Texture create2DMultisample(ImageFormat fmt, int w, int h, Samples ms = Samples{ 0 });
  static Texture create3D(ImageFormat fmt, int w, int h, int d, MipMaps mipMaps = MipMaps{ 1 });

private:
  GLHandle<TextureDeleter> obj_;
  int numSamples_{0};
  ImageDesc desc_;
};

int getTextureMipMapCount(int width, int height);

struct GLFormatInfo {
  gl::GLenum internal_fmt;
  gl::GLenum external_fmt;
  gl::GLenum type;
  int num_comp;
  int size;
};

const GLFormatInfo &getGLImageFormatInfo(ImageFormat fmt);
}

