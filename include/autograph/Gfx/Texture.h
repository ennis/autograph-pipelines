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

///
/// Wrapper for OpenGL textures
/// To create a 'Texture' object, use the constructor with a Texture::Desc
/// object describing the texture, or use one of the named constructors
/// (createnD(...))
/// The underlying GL texture object is created with immutable storage, meaning
/// that it is impossible to reallocate the storage (resizing, adding mip
/// levels) one the texture is created
class AG_GFX_API Texture {
public:
  /// Used in the named constructors to specify the number of mip map levels to
  /// allocate
  struct MipMaps {
    int count;
  };

  /// Used in the named constructors to create multisampled textures
  /// if count == 0, then multisampling is disabled, else, a multisampled
  /// texture is created, with 'count' samples
  struct Samples {
    int count;
  };

  /// Option flags for texture creation
  enum class Options {
    SparseStorage = (1 << 0) ///< Do not allocate physical storage for the
    ///< texture on creation (see documentation for OpenGL
    ///< sparse textures, and commit/decommitTiledRegion)
  };

  /// Description of a texture used during creation
  struct Desc {
    ImageDimensions dims; ///< Dimensions (1D,2D,3D,Array,Cube map...)
    ImageFormat fmt;      ///< Format, one of the ImageFormats
    int width;            ///< width
    int height;           ///< height, should be 1 for 1D textures
    int depth;            ///< depth, should be 1 for 1D or 2D textures
    int sampleCount;      ///< Number of samples. Setting sampleCount >= 1 will
                          ///< create a multisampled texture
    int mipMapCount;      ///< Number of mip maps
    Options opts; ///< Texture creation flags. See Texture::Options for more
                  ///< information.

    /// Returns the hash of the texture description
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

	/// Comparison operator for Texture.Desc
    constexpr bool operator==(const Desc &rhs) const {
      return dims == rhs.dims && fmt == rhs.fmt && width == rhs.width &&
             height == rhs.height && depth == rhs.depth &&
             sampleCount == rhs.sampleCount && mipMapCount == rhs.mipMapCount &&
             opts == rhs.opts;
    }
  };

  /// Default constructor: creates a null texture object
  /// (i.e. OpenGL object 0)
  Texture() = default;
  /// Create a texture from the specified description
  Texture(const Desc &desc);
  /// Default move constructors (this disables copy constructors)
  Texture(Texture &&rhs) = default;
  Texture &operator=(Texture &&rhs) = default;

  /// Destructor
  /// if a texture has been allocated for this object, then it is deleted
  /// by calling the corresponding glDeleteXXX function
  ~Texture();

  //====================================
  // Texture properties

  /// Returns the width (x-resolution) of the texture
  auto width() const { return desc_.width; }
  /// Returns the height (y-resolution) of the texture, or 1 if it's a 1D
  /// texture
  auto height() const { return desc_.height; }
  /// Returns the depth (z-resolution) of the texture, or 1 if it's a 1D or 2D
  /// texture
  auto depth() const { return desc_.depth; }
  /// Returns the size (width, height, depth) of the specified mip map level of
  /// the texture
  /// (defaults to mip map level 0)
  auto size(int mip_level = 0) const {
    return ivec3{desc_.width, desc_.height, desc_.depth};
  }
  /// Returns the storage format of the texture data
  auto format() const { return desc_.fmt; }
  /// Returns the tile size (width, height, depth) of the texture, i.e., the
  /// size of the physical memory pages backing a sparse texture.
  /// It is also the granularity of the 'commitTiledRegion' and
  /// 'decommitTiledRegion' operations.
  ivec3 getTileSize();
  /// Generate mip maps for the texture from the highest (level 0) level.
  /// Note that this function does not allocate new mip levels. It only fills
  /// the mip levels allocated at texture creation time.
  /// See glGenerateMipMaps for more information.
  void generateMipmaps();
  /// Returns a description of the texture object.
  /// See Texture::Desc
  const auto &desc() const { return desc_; }
  /// Returns the wrapped GL texture object
  /// Returns 0 for the null texture (texture created with the default
  /// constructor, or moved-from textures)
  gl::GLuint object() const { return obj_.get(); }
  /// Returns true if this texture is not the null texture
  explicit operator bool() const { return obj_.get() != 0; }
  /// Free the GL texture object and become the null texture
  void reset();

  //====================================
  // Pixel transfer

  /// Upload pixel data to the texture
  /// TODO this is unsafe: specify format of input buffer data
  void upload(void *src, int mipLevel = 0);
  /// Read back pixel data from the texture
  /// TODO this is unsafe: specify format of output buffer data
  void get(void *dest, int mipLevel = 0);
  /// Read back pixel data from a region of the texture
  /// TODO this is unsafe: specify format of output buffer data
  void getRegion(void *dest, int x, int y, int width, int height,
                 int mipLevel = 0);
  /// Fetch one texel of the texture at the specified coordinates and mip level
  vec4 texelFetch(ivec3 coords, int mip_level = 0);

  //====================================
  // commitment for sparse textures

  /// Commit physical memory pages for the texture at the specified region on
  /// the specified mip map level
  /// tileCoords and regionSize must be integer multiples of the sparse tile
  /// size
  /// obtained with getTileSize()
  /// See also glTexPageCommitmentARB
  /// Needs ARB_sparse_texture extension
  void commitTiledRegion(int mipLevel, ivec3 tileCoords, ivec3 regionSize);

  /// Decommit physical memory pages for the texture at the specified region on
  /// the specified mip map level
  /// tileCoords and regionSize must be integer multiples of the sparse tile
  /// size
  /// obtained with getTileSize()
  /// See also glTexPageCommitmentARB
  /// Needs ARB_sparse_texture extension
  void decommitTiledRegion(int mipLevel, ivec3 tileCoords, ivec3 regionSize);

  //====================================
  // Named constructors

  /// Create a 1D texture
  /// See also Texture::MipMaps, Texture::Options
  static Texture create1D(ImageFormat fmt, int w, MipMaps mipMaps = MipMaps{1},
                          Options opts = (Options)0);
  /// Create a 2D texture. If ms.count != 0, a multisample texture is created
  /// See also Texture::MipMaps, Texture::Options
  static Texture create2D(ImageFormat fmt, int w, int h,
                          MipMaps mipMaps = MipMaps{1}, Samples ms = Samples{0},
                          Options opts = (Options)0);
  /// Create a 3D texture
  /// See also Texture::MipMaps, Texture::Options
  static Texture create3D(ImageFormat fmt, int w, int h, int d,
                          MipMaps mipMaps = MipMaps{1},
                          Options opts = (Options)0);

private:
  Desc desc_;
  GLHandle<TextureDeleter> obj_;
  gl::GLenum target_ = gl::TEXTURE_2D;
};

ENUM_BIT_FLAGS_OPERATORS(Texture::Options)

/// Get the maximum number of mip map levels for a 2D texture of size
/// (width,height)
/// numLevels = 1 + floor(log2(max(w, h, d)))
/// See:
/// https://stackoverflow.com/questions/9572414/how-many-mipmaps-does-a-texture-have-in-opengl
int getTextureMipMapCount(int width, int height);

/// Structure containing information about the OpenGL internal format
/// corresponding to an 'ImageFormat'
struct GLFormatInfo {
  gl::GLenum internal_fmt; //< Corresponding internal format
  gl::GLenum external_fmt; //< Preferred external format for uploads/reads
  gl::GLenum type;         //< Preferred element type for uploads/reads
  int num_comp;            //< number of components (channels) (TODO redundant)
  int size;                //< Size of one pixel in bytes
};

/// Returns information about the OpenGL internal format corresponding to the
/// specified ImageFormat
const GLFormatInfo &getGLImageFormatInfo(ImageFormat fmt);

} // namespace ag
