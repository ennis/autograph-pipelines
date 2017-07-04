#include <algorithm>
#include <autograph/Core/Support/Debug.h>
#include <autograph/Gfx/Texture.h>
#include <cassert>
#include <cmath>
#include <stdexcept>

namespace ag {

void TextureDeleter::operator()(gl::GLuint tex_obj) {
  gl::DeleteTextures(1, &tex_obj);
}

Texture::~Texture() {
  /*if (obj_.get() != 0)
    AG_DEBUG("Deleting texture {}, object={} ({},{}x{}x{})", (const void *)this,
             obj_.get(), getImageFormatInfo(format()).name, width(), height(),
             depth());*/
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
