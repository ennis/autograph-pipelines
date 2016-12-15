#include <autograph/gl/Texture.h>
#include <autograph/support/Debug.h>
#include <cassert>
#include <stdexcept>

namespace ag {
namespace gl {
static GLFormatInfo glfmt_rgba8_unorm{GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, 4,
                                      4};
static GLFormatInfo glfmt_r8_unorm{GL_R8, GL_RED, GL_UNSIGNED_BYTE, 1, 1};
static GLFormatInfo glfmt_r32_float{GL_R32F, GL_RED, GL_FLOAT, 1, 4};
static GLFormatInfo glfmt_rg32_float{GL_RG32F, GL_RG, GL_FLOAT, 2, 8};
static GLFormatInfo glfmt_rgba32_float{GL_RGBA32F, GL_RGBA, GL_FLOAT, 4, 16};
static GLFormatInfo glfmt_depth32_float{GL_DEPTH_COMPONENT32F,
                                        GL_DEPTH_COMPONENT, GL_FLOAT, 1, 4};
static GLFormatInfo glfmt_argb_10_10_10_2_unorm{GL_RGB10_A2,
                                        GL_DEPTH_COMPONENT, GL_FLOAT, 1, 4};

void TextureDeleter::operator()(GLuint tex_obj) { 
	glDeleteTextures(1, &tex_obj); 
}

const GLFormatInfo &getGLImageFormatInfo(ImageFormat fmt) {
  switch (fmt) {
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
  case ImageFormat::A2R10G10B10_SNORM_PACK32:
      //return glfmt_argb_10_10_10_2_snorm;   // there is no signed version of this format in OpenGL
  default:
    throw std::logic_error("Unsupported image format");
  }
}

Texture::Texture(const ImageDesc &desc) : desc_{desc}
{
  assert(desc.numMipmaps > 0);
  GLuint tex_obj;
  const auto &glfmt = getGLImageFormatInfo(desc.format);
  switch (desc.dimensions) {
  case ImageDimensions::Image1D:
    glCreateTextures(GL_TEXTURE_1D, 1, &tex_obj);
    glTextureStorage1D(tex_obj, desc.numMipmaps, glfmt.internal_fmt,
                       desc.width);
    break;
  case ImageDimensions::Image2D:
    glCreateTextures(GL_TEXTURE_2D, 1, &tex_obj);
    glTextureStorage2D(tex_obj, desc.numMipmaps, glfmt.internal_fmt, desc.width,
                       desc.height);
    break;
  case ImageDimensions::Image3D:
    glCreateTextures(GL_TEXTURE_3D, 1, &tex_obj);
    glTextureStorage3D(tex_obj, desc.numMipmaps, glfmt.internal_fmt, desc.width,
                       desc.height, desc.depth);
    break;
  default:
    throw std::logic_error("Unsupported texture type");
  }
  // set sensible defaults
  glTextureParameteri(tex_obj, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTextureParameteri(tex_obj, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTextureParameteri(tex_obj, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  obj_ = GLHandle<TextureDeleter>{tex_obj};
}

Texture::~Texture()
{
	if (obj_.get() != 0)
		AG_DEBUG("Deleting texture {}, object={} ({},{}x{}x{})", (const void*)this, obj_.get(), getImageFormatInfo(format()).name, width(), height(), depth());
}

void Texture::upload(void *src, int mipLevel) {
  auto gl_fmt = getGLImageFormatInfo(desc_.format);
  switch (desc_.dimensions) {
  case ImageDimensions::Image1D:
    glTextureSubImage1D(obj_.get(), mipLevel, 0, desc_.width,
                        gl_fmt.external_fmt, gl_fmt.type, src);
  case ImageDimensions::Image2D:
    glTextureSubImage2D(obj_.get(), mipLevel, 0, 0, desc_.width, desc_.height,
                        gl_fmt.external_fmt, gl_fmt.type, src);
  case ImageDimensions::Image3D:
    glTextureSubImage3D(obj_.get(), mipLevel, 0, 0, 0, desc_.width,
                        desc_.height, desc_.depth, gl_fmt.external_fmt,
                        gl_fmt.type, src);
  }
}

void Texture::get(void *dest, int mipLevel) {
  auto gl_fmt = getGLImageFormatInfo(desc_.format);
  glGetTextureImage(obj_.get(), mipLevel, gl_fmt.external_fmt, gl_fmt.type,
                    gl_fmt.size * desc_.width * desc_.height * desc_.depth,
                    dest);
}

void Texture::generateMipmaps() { glGenerateTextureMipmap(obj_.get()); }

void Texture::getRegion(void *dest, int x, int y, int width,
	int height, int mipLevel) {
  auto gl_fmt = getGLImageFormatInfo(desc_.format);
  glGetTextureSubImage(obj_.get(), mipLevel, x, y, 0, width, height, 1,
                       gl_fmt.external_fmt, gl_fmt.type,
                       gl_fmt.size * width * height, dest);
}

glm::vec4 Texture::texelFetch(glm::ivec3 coords, int mip_level) {
  glm::vec4 out;
  glGetTextureSubImage(obj_.get(), mip_level, coords.x, coords.y, coords.z, 1,
                       1, 1, GL_RGBA, GL_FLOAT, 4 * 4, &out);
  return out;
}

Texture Texture::create1D(ImageFormat fmt, int w, int numMipmaps) {
  ImageDesc d;
  d.dimensions = ImageDimensions::Image1D;
  d.format = fmt;
  d.width = w;
  d.height = 1;
  d.depth = 1;
  d.numMipmaps = numMipmaps;
  return Texture{d};
}

Texture Texture::create2D(ImageFormat fmt, int w, int h,
	int numMipmaps) {
  ImageDesc d;
  d.dimensions = ImageDimensions::Image2D;
  d.format = fmt;
  d.width = w;
  d.height = h;
  d.depth = 1;
  d.numMipmaps = numMipmaps;
  return Texture{d};
}

void Texture::reset()
{
	if (obj_.get() != 0)
		AG_DEBUG("Deleting texture {}, object={} ({},{}x{}x{})", (const void*)this, obj_.get(), getImageFormatInfo(format()).name, width(), height(), depth());
    obj_ = nullptr;
}

}
}
