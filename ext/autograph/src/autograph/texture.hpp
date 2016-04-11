#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include "pixel_format.hpp"
#include "gl_types.hpp"

namespace ag {
enum class TextureAddressMode { Repeat, Clamp, Mirror };

enum class TextureFilter { Nearest, Linear };

////////////////////////// Sampler
struct SamplerInfo {
  TextureAddressMode addrU = TextureAddressMode::Repeat;
  TextureAddressMode addrV = TextureAddressMode::Repeat;
  TextureAddressMode addrW = TextureAddressMode::Repeat;
  TextureFilter minFilter = TextureFilter::Nearest;
  TextureFilter magFilter = TextureFilter::Linear;
};

struct sampler_deleter {
  void operator()(GLuint sampler_obj) { gl::DeleteSamplers(1, &sampler_obj); }
};

struct Sampler {
  gl_handle<sampler_deleter> obj;
  SamplerInfo info;
};

struct texture_deleter {
  void operator()(GLuint tex_obj) { gl::DeleteTextures(1, &tex_obj); }
};

enum class texture_dimension { tex_1d, tex_2d, tex_3d };

struct TextureBase {
  gl_handle<texture_deleter> obj;
  PixelFormat format;
  texture_dimension dimension;
  unsigned width;
  unsigned height;
  unsigned depth;
};

////////////////////////// Texture1D
template <typename T> struct Texture1D : public TextureBase {
  Texture1D() = default;
  Texture1D(GLuint obj, PixelFormat format, unsigned width_)
      : TextureBase{obj, format, texture_dimension::tex_1d, width_, 1, 1} {}
};

////////////////////////// Texture2D
template <typename T> struct Texture2D : public TextureBase {
  Texture2D() = default;
  Texture2D(GLuint obj, PixelFormat format, unsigned width_, unsigned height_)
      : TextureBase{obj, format, texture_dimension::tex_2d, width_, height_,
                    1} {}
};

////////////////////////// Texture3D
template <typename T> struct Texture3D : public TextureBase {
  Texture3D() = default;
  Texture3D(GLuint obj, PixelFormat format, unsigned width_, unsigned height_,
            unsigned depth_)
      : TextureBase{obj, format, texture_dimension::tex_3d, width_, height_,
                    depth_} {}
};
}

#endif // !TEXTURE_HPP
