#include <algorithm>
#include <autograph/Core/Support/Debug.h>
#include <autograph/Gfx/Texture.h>
#include <cassert>
#include <cmath>
#include <stdexcept>

namespace ag {

static GLFormatInfo glfmt_rgba8_unorm{gl::RGBA8, gl::RGBA, gl::UNSIGNED_BYTE, 4,
                                      4};
static GLFormatInfo glfmt_r8_unorm{gl::R8, gl::RED, gl::UNSIGNED_BYTE, 1, 1};
static GLFormatInfo glfmt_r32_float{gl::R32F, gl::RED, gl::FLOAT, 1, 4};
static GLFormatInfo glfmt_rg32_float{gl::RG32F, gl::RG, gl::FLOAT, 2, 8};
static GLFormatInfo glfmt_rgba16_float{gl::RGBA16F, gl::RGBA, gl::FLOAT, 4, 8};
static GLFormatInfo glfmt_rgba32_float{gl::RGBA32F, gl::RGBA, gl::FLOAT, 4, 16};
static GLFormatInfo glfmt_rgba32_uint{gl::RGBA32UI, gl::RGBA, gl::UNSIGNED_INT,
                                      4, 16};
static GLFormatInfo glfmt_depth32_float{gl::DEPTH_COMPONENT32F,
                                        gl::DEPTH_COMPONENT, gl::FLOAT, 1, 4};
static GLFormatInfo glfmt_argb_10_10_10_2_unorm{
    gl::RGB10_A2, gl::DEPTH_COMPONENT, gl::FLOAT, 1, 4};
static GLFormatInfo glfmt_rgba8_unorm_srgb{gl::SRGB8_ALPHA8, gl::RGBA,
                                           gl::UNSIGNED_BYTE, 4, 4};
static GLFormatInfo glfmt_rg16_float{gl::RG16F, gl::RG, gl::FLOAT, 2, 4};
static GLFormatInfo glfmt_rg16_sint{gl::RG16I, gl::RG, gl::INT, 2, 4};

void TextureDeleter::operator()(gl::GLuint tex_obj) {
  gl::DeleteTextures(1, &tex_obj);
}

const GLFormatInfo &getGLImageFormatInfo(ImageFormat fmt) {
  switch (fmt) {
  case ImageFormat::R32G32B32A32_UINT:
    return glfmt_rgba32_uint;
  case ImageFormat::R16G16B16A16_SFLOAT:
    return glfmt_rgba16_float;
  case ImageFormat::R8G8B8A8_UNORM:
    return glfmt_rgba8_unorm;
  case ImageFormat::R8G8B8A8_SNORM:
    return glfmt_r8_unorm;
  case ImageFormat::R32_SFLOAT:
    return glfmt_r32_float;
  case ImageFormat::R32G32_SFLOAT:
    return glfmt_rg32_float;
  case ImageFormat::R32G32B32A32_SFLOAT:
    return glfmt_rgba32_float;
  case ImageFormat::D32_SFLOAT:
    return glfmt_depth32_float;
  case ImageFormat::A2R10G10B10_UNORM_PACK32:
    return glfmt_argb_10_10_10_2_unorm;
  case ImageFormat::R8G8B8A8_SRGB:
    return glfmt_rgba8_unorm_srgb;
  case ImageFormat::R16G16_SFLOAT:
    return glfmt_rg16_float;
  case ImageFormat::R16G16_SINT:
    return glfmt_rg16_sint;
  case ImageFormat::A2R10G10B10_SNORM_PACK32:
  // return glfmt_argb_10_10_10_2_snorm;   // there is no signed version of this
  // format in OpenGL
  default:
    throw std::logic_error("Unsupported image format");
  }
}

/*Texture::Texture(const ImageDesc &desc) : desc_{desc}
{
}*/

Texture::~Texture() {
  if (obj_.get() != 0)
    AG_DEBUG("Deleting texture {}, object={} ({},{}x{}x{})", (const void *)this,
             obj_.get(), getImageFormatInfo(format()).name, width(), height(),
             depth());
}

void Texture::upload(void *src, int mipLevel) {
  auto gl_fmt = getGLImageFormatInfo(desc_.fmt);
  switch (desc_.dims) {
  case ImageDimensions::Image1D:
    gl::TextureSubImage1D(obj_.get(), mipLevel, 0, desc_.width,
                          gl_fmt.external_fmt, gl_fmt.type, src);
    break;
  case ImageDimensions::Image2D:
    gl::TextureSubImage2D(obj_.get(), mipLevel, 0, 0, desc_.width, desc_.height,
                          gl_fmt.external_fmt, gl_fmt.type, src);
    break;
  case ImageDimensions::Image3D:
    gl::TextureSubImage3D(obj_.get(), mipLevel, 0, 0, 0, desc_.width,
                          desc_.height, desc_.depth, gl_fmt.external_fmt,
                          gl_fmt.type, src);
    break;
  }
}

void Texture::get(void *dest, int mipLevel) {
  auto gl_fmt = getGLImageFormatInfo(desc_.fmt);
  gl::GetTextureImage(obj_.get(), mipLevel, gl_fmt.external_fmt, gl_fmt.type,
                      gl_fmt.size * desc_.width * desc_.height * desc_.depth,
                      dest);
}

void Texture::generateMipmaps() { gl::GenerateTextureMipmap(obj_.get()); }

void Texture::getRegion(void *dest, int x, int y, int width, int height,
                        int mipLevel) {
  auto gl_fmt = getGLImageFormatInfo(desc_.fmt);
  gl::GetTextureSubImage(obj_.get(), mipLevel, x, y, 0, width, height, 1,
                         gl_fmt.external_fmt, gl_fmt.type,
                         gl_fmt.size * width * height, dest);
}

glm::vec4 Texture::texelFetch(glm::ivec3 coords, int mip_level) {
  glm::vec4 out;
  gl::GetTextureSubImage(obj_.get(), mip_level, coords.x, coords.y, coords.z, 1,
                         1, 1, gl::RGBA, gl::FLOAT, 4 * 4, &out);
  return out;
}

Texture::Texture(const Desc &desc) : desc_{desc} {
  switch (desc.dims) {
  case ImageDimensions::Image1D:
    target_ = gl::TEXTURE_1D;
    break;
  case ImageDimensions::Image2D:
    if (desc.sampleCount > 0) {
		target_ = gl::TEXTURE_2D_MULTISAMPLE;
    } else {
		target_ = gl::TEXTURE_2D;
    }
    break;
  case ImageDimensions::Image3D:
	  target_ = gl::TEXTURE_3D;
    break;
  }

  const auto &glfmt = getGLImageFormatInfo(desc.fmt);
  gl::GLuint tex_obj;
  gl::CreateTextures(target_, 1, &tex_obj);
  if (!!(desc.opts & Options::SparseStorage)) {
    gl::TextureParameteri(tex_obj, gl::TEXTURE_SPARSE_ARB, gl::TRUE_);
  }

  switch (target_) {
  case gl::TEXTURE_1D:
    gl::TextureStorage1D(tex_obj, desc.mipMapCount, glfmt.internal_fmt,
                         desc.width);
    break;
  case gl::TEXTURE_2D:
    gl::TextureStorage2D(tex_obj, desc.mipMapCount, glfmt.internal_fmt,
                         desc.width, desc.height);
    break;
  case gl::TEXTURE_2D_MULTISAMPLE:
    gl::TextureStorage2DMultisample(tex_obj, desc.sampleCount,
                                    glfmt.internal_fmt, desc.width, desc.height,
                                    true);
    break;
  case gl::TEXTURE_3D:
    gl::TextureStorage3D(tex_obj, 1, glfmt.internal_fmt, desc.width,
                         desc.height, desc.depth);
    break;
  }
  // set sensible defaults
  gl::TextureParameteri(tex_obj, gl::TEXTURE_WRAP_S, gl::CLAMP_TO_EDGE);
  gl::TextureParameteri(tex_obj, gl::TEXTURE_WRAP_T, gl::CLAMP_TO_EDGE);
  gl::TextureParameteri(tex_obj, gl::TEXTURE_WRAP_R, gl::CLAMP_TO_EDGE);
  gl::TextureParameteri(tex_obj, gl::TEXTURE_MIN_FILTER, gl::NEAREST);
  gl::TextureParameteri(tex_obj, gl::TEXTURE_MAG_FILTER, gl::NEAREST);
  // fill desc
  obj_ = tex_obj;
}

ivec3 Texture::getTileSize() {
  const auto &glfmt = getGLImageFormatInfo(desc_.fmt);
  ivec3 tileSize;
  // query tile size
  gl::GetInternalformativ(target_, glfmt.internal_fmt,
                          gl::VIRTUAL_PAGE_SIZE_X_ARB, 1, &tileSize.x);
  gl::GetInternalformativ(target_, glfmt.internal_fmt,
                          gl::VIRTUAL_PAGE_SIZE_Y_ARB, 1, &tileSize.y);
  gl::GetInternalformativ(target_, glfmt.internal_fmt,
                          gl::VIRTUAL_PAGE_SIZE_Z_ARB, 1, &tileSize.z);
  return tileSize;
}

Texture Texture::create1D(ImageFormat fmt, int w, MipMaps mipMaps,
                          Options opts) {
  Desc desc{ImageDimensions::Image1D, fmt, w, 1, 1, 0, mipMaps.count, opts};
  return Texture{desc};
}

Texture Texture::create2D(ImageFormat fmt, int w, int h, MipMaps mipMaps,
                          Samples ms, Options opts) {
  Desc desc{
      ImageDimensions::Image2D, fmt, w, h, 1, ms.count, mipMaps.count, opts};
  return Texture{desc};
}

Texture Texture::create3D(ImageFormat fmt, int w, int h, int d, MipMaps mipMaps,
                          Options opts) {
  Desc desc{ImageDimensions::Image3D, fmt, w, h, d, 0, mipMaps.count, opts};
  return Texture{desc};
}

void Texture::commitTiledRegion(int mipLevel, ivec3 tileCoords,
                                ivec3 regionSize) {
  gl::BindTexture(target_, obj_.get());
  gl::TexPageCommitmentARB(target_, mipLevel, tileCoords.x, tileCoords.y,
                           tileCoords.z, regionSize.x, regionSize.y,
                           regionSize.z, true);
}

void Texture::decommitTiledRegion(int mipLevel, ivec3 tileCoords,
                                  ivec3 regionSize) {
  gl::BindTexture(target_, obj_.get());
  gl::TexPageCommitmentARB(target_, mipLevel, tileCoords.x, tileCoords.y,
                           tileCoords.z, regionSize.x, regionSize.y,
                           regionSize.z, false);
}

void Texture::reset() {
  if (obj_.get() != 0)
    AG_DEBUG("Deleting texture {}, object={} ({},{}x{}x{})", (const void *)this,
             obj_.get(), getImageFormatInfo(format()).name, width(), height(),
             depth());
  obj_ = nullptr;
}

int getTextureMipMapCount(int width, int height) {
  // 1000 is the default value of GL_TEXTURE_MAX_LEVEL
  return std::min((int)std::floor(std::log2(std::max(width, height))), 1000) -
         1;
}

} // namespace ag
