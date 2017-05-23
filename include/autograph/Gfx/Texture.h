#pragma once
#include <autograph/Core/ImageDesc.h>
#include <autograph/Core/ImageFormat.h>
#include <autograph/Core/Support/Flags.h>
#include <autograph/Core/Support/HashCombine.h>
#include <autograph/Core/Types.h>
#include <autograph/Gfx/GLHandle.h>

namespace ag {

struct AG_GFX_API TextureDeleter {
  static constexpr gl::GLenum objectType = gl::TEXTURE;
  void operator()(gl::GLuint tex_obj);
};

class AG_GFX_API Texture {
public:
  //////////////////////////////////
  struct MipMaps {
    int count;
  };
  struct Samples {
    int count;
  };
  enum class Options { SparseStorage = (1 << 0) };

  struct Desc {
    ImageDimensions dims;
    ImageFormat fmt;
    int width;
    int height;
    int depth;
    int sampleCount;
    int mipMapCount;
    Options opts;

    constexpr size_t hash() const {
      size_t h = 0;
      hashCombine(h, static_cast<std::underlying_type_t<decltype(dims)>>(dims));
      hashCombine(h, static_cast<std::underlying_type_t<decltype(fmt)>>(fmt));
      hashCombine(h, width);
      hashCombine(h, height);
      hashCombine(h, depth);
      hashCombine(h, sampleCount);
      hashCombine(h, mipMapCount);
      hashCombine(h, static_cast<std::underlying_type_t<decltype(opts)>>(opts));
      return h;
    }

    constexpr bool operator==(const Desc &rhs) const {
      return dims == rhs.dims && fmt == rhs.fmt && width == rhs.width &&
             height == rhs.height && depth == rhs.depth &&
             sampleCount == rhs.sampleCount && mipMapCount == rhs.mipMapCount &&
             opts == rhs.opts;
    }
  };

  Texture() = default;
  Texture(const Desc &desc);
  Texture(Texture &&rhs) = default;
  Texture &operator=(Texture &&rhs) = default;

  ~Texture();

  //====================================
  // Texture properties
  auto width() const { return desc_.width; }
  auto height() const { return desc_.height; }
  auto depth() const { return desc_.depth; }
  auto size(int mip_level = 0) const {
    return ivec3{desc_.width, desc_.height, desc_.depth};
  }
  auto format() const { return desc_.fmt; }
  ivec3 getTileSize();
  void generateMipmaps();
  const auto &desc() const { return desc_; }
  gl::GLuint object() const { return obj_.get(); }
  explicit operator bool() const { return obj_.get() != 0; }
  void reset();

  //====================================
  // Pixel transfer
  void upload(void *src, int mipLevel = 0);
  void get(void *dest, int mipLevel = 0);
  void getRegion(void *dest, int x, int y, int width, int height,
                 int mipLevel = 0);
  vec4 texelFetch(ivec3 coords, int mip_level = 0);

  //====================================
  // commitment for sparse textures
  // tile pos and size must be integer multiples of the sparse tile size
  // obtained with
  void commitTiledRegion(int mipLevel, ivec3 tileCoords, ivec3 regionSize);
  void decommitTiledRegion(int mipLevel, ivec3 tileCoords, ivec3 regionSize);

  //====================================
  // Named constructors
  static Texture create1D(ImageFormat fmt, int w, MipMaps mipMaps = MipMaps{1},
                          Options opts = (Options)0);
  static Texture create2D(ImageFormat fmt, int w, int h,
                          MipMaps mipMaps = MipMaps{1}, Samples ms = Samples{0},
                          Options opts = (Options)0);
  static Texture create3D(ImageFormat fmt, int w, int h, int d,
                          MipMaps mipMaps = MipMaps{1},
                          Options opts = (Options)0);

private:
  // prefer using the named constructors above for a more
  // readable alternative
  Texture(gl::GLenum target, ImageFormat fmt, int w, int h, int d,
          int mipMapCount = 1, int sampleCount = 0, Options opts = (Options)0);

  Desc desc_;
  GLHandle<TextureDeleter> obj_;
  gl::GLenum target_ = gl::TEXTURE_2D;
};

ENUM_BIT_FLAGS_OPERATORS(Texture::Options)

int getTextureMipMapCount(int width, int height);

struct GLFormatInfo {
  gl::GLenum internal_fmt;
  gl::GLenum external_fmt;
  gl::GLenum type;
  int num_comp;
  int size;
};

const GLFormatInfo &getGLImageFormatInfo(ImageFormat fmt);
} // namespace ag
