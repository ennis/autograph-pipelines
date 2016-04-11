#ifndef COPY_HPP
#define COPY_HPP

#include "buffer.hpp"
#include "device.hpp"
#include "pixel_format.hpp"
#include "rect.hpp"
#include "texture.hpp"

namespace ag {

///////////////////// CPU -> Texture
// CPU -> Texture1D
template <typename Pixel>
void copy(Device& device, void* src, Texture1D<Pixel>& dest,
          unsigned mipLevel = 0) {
  auto gl_fmt = pixelFormatToGL(dest.format);
  gl::TextureSubImage1D(dest.obj.get(), mipLevel, 0, dest.width,
                        gl_fmt.externalFormat, gl_fmt.type, src);
}

// CPU -> Texture2D
template <typename Pixel>
void copy(Device& device, void* src, Texture2D<Pixel>& dest,
          unsigned mipLevel = 0) {
  auto gl_fmt = pixelFormatToGL(dest.format);
  gl::TextureSubImage2D(dest.obj.get(), mipLevel, 0, 0, dest.width, dest.height,
                        gl_fmt.externalFormat, gl_fmt.type, src);
}

// CPU -> Texture3D
template <typename Pixel>
void copy(Device& device, void* src, Texture3D<Pixel>& dest,
          unsigned mipLevel = 0) {
  auto gl_fmt = pixelFormatToGL(dest.format);
  gl::TextureSubImage3D(dest.obj.get(), mipLevel, 0, 0, 0, dest.width, dest.height,
                        dest.depth, gl_fmt.externalFormat, gl_fmt.type, src);
}

// Texture1D -> CPU
template <typename Pixel>
void copySync(Device& device, Texture1D<Pixel>& src, void* dest,
              unsigned mipLevel = 0) {
  auto gl_fmt = pixelFormatToGL(src.format);
  gl::GetTextureImage(src.obj.get(), mipLevel, gl_fmt.externalFormat,
                      gl_fmt.type, sizeof(Pixel) * src.width, dest);
}

// Texture2D -> CPU
template <typename Pixel>
void copySync(Device& device, Texture2D<Pixel>& src, void* dest,
              unsigned mipLevel = 0) {
  auto gl_fmt = pixelFormatToGL(src.format);
  gl::GetTextureImage(src.obj.get(), mipLevel, gl_fmt.externalFormat,
                      gl_fmt.type, sizeof(Pixel) * src.width * src.height,
                      dest);
}

// Texture1D -> CPU
template <typename Pixel>
void copyRegionSync(Device& device, Texture1D<Pixel>& src, void* dest, unsigned x, unsigned width, unsigned mipLevel = 0) {
  auto gl_fmt = pixelFormatToGL(src.format);
  gl::GetTextureSubImage(src.obj.get(), mipLevel, x, 0, 0, width, 1, 1, gl_fmt.externalFormat,
                      gl_fmt.type, sizeof(Pixel) * src.width, dest);
}

// Texture2D -> CPU
template <typename Pixel>
void copyRegionSync(Device& device, Texture2D<Pixel>& src, void* dest, unsigned x, unsigned y, unsigned width, unsigned height,
              unsigned mipLevel = 0) {
  auto gl_fmt = pixelFormatToGL(src.format);
  gl::GetTextureSubImage(src.obj.get(), mipLevel, x, y, 0, width, height, 1, gl_fmt.externalFormat,
                      gl_fmt.type, sizeof(Pixel) * width * height,
                      dest);
}

///////////////////// Texture -> buffer copy operations
/*template <typename D, typename Pixel,
          typename Storage = typename PixelTypeTraits<Pixel>::storage_type>
void copy(Device<D>& device, Texture1D<Pixel, D>& texture,
          RawBufferSlice<D>& buffer, const ag::Box1D& region,
          unsigned mipLevel = 0) {
  // TODO should check that the Storage and Buffer types are compatible
  device.backend.copyTextureRegion1D(texture, buffer, region, mipLevel);
}

template <typename D, typename Pixel,
          typename Storage = typename PixelTypeTraits<Pixel>::storage_type>
void copy(Device<D>& device, Texture2D<Pixel, D>& texture,
          RawBufferSlice<D>& buffer, const ag::Box2D& region,
          unsigned mipLevel = 0) {
  // TODO should check that the Storage and Buffer types are compatible
  device.backend.copyTextureRegion2D(texture, buffer, region, mipLevel);
}*/
}

#endif
