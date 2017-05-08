/*#include <ImfInputFile.h>
#include <ImfRgbaFile.h>
#include <ImfArray.h>*/
#include <autograph/Engine/Application.h>
#include <autograph/Engine/ImageUtils.h>
#include <autograph/Core/Support/Debug.h>
#include <experimental/filesystem>
#include <OpenImageIO/imageio.h>

namespace fs = std::experimental::filesystem;

static void *stbi_realloc_sized(void *ptr, size_t oldsz, size_t newsz) {
  auto ptr2 = new uint8_t[newsz];
  memcpy(ptr2, ptr, oldsz);
  delete[](uint8_t *)(ptr);
  return ptr2;
}

#define STBI_MALLOC(sz) ((void *)(new uint8_t[sz]))
#define STBI_REALLOC_SIZED(ptr, oldsz, newsz)                                  \
  (stbi_realloc_sized(ptr, oldsz, newsz))
#define STBI_FREE(ptr) (delete[](uint8_t *)(ptr))

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace ag {
static std::unique_ptr<uint8_t[]> loadImageByPathRaw(const char *path,
                                                     int &width, int &height,
                                                     int &comp, int req_cmp) 
{
  auto raw_data = stbi_load(path, &width, &height, &comp, req_cmp);
  if (!raw_data) {
    errorMessage("Missing, corrupt image file, or unsupported format: {}",
                 path);
    throw std::runtime_error{
        "Missing, corrupt image file, or unsupported format"};
  }
  return std::unique_ptr<uint8_t[]>{raw_data};
}

Image loadImage(const char *id, ImageFormat targetFormat) {
  // TODO DDS loader
  // TODO mip maps, subresources
  // TODO faces
  // TODO FP formats
  // TODO conversions
  auto path = ResourceManager::getFilesystemPath(id);
  if (path.empty()) {
    throw std::runtime_error{"loadImage: file not found"};
  }
  Image img;
  int comp;
  // TODO filter out unsupported formats
  img.data = loadImageByPathRaw(path.c_str(), img.desc.width, img.desc.height,
                                comp, 4);
  img.desc.numMipmaps = 1;
  img.desc.dimensions = ImageDimensions::Image2D;
  img.desc.depth = 1;
  img.desc.format = targetFormat;
  return img;
}


Texture loadTexture(const char *path, ImageFormat targetFormat) 
{
  auto fspath = ResourceManager::getFilesystemPath(path);
  if (fspath.empty()) {
	  errorMessage("loadTexture: {}: path not found", path);
	return {};
  }

  using namespace OIIO;
  ImageInput* in = ImageInput::open(fspath);
  if (!in) {
	  errorMessage("loadTexture: error loading image at {}", fspath.c_str());
	  return {};
  }

  const auto& spec = in->spec();
  if (spec.nchannels < 1 && spec.nchannels > 4) {
	  errorMessage("loadTexture: invalid number of channels {}", fspath.c_str());
	  return {};
  }

  AG_DEBUG("Loading image {}x{} nchannels {} format {}", spec.width, spec.height, spec.nchannels, spec.format.c_str());
  
  // So the goal here is to load the contents of the image file into a texture
  // First, we have to load it in a temporary buffer
  // The pipeline is as follows:
  //
  //             OIIO               OpenGL  
  //     file     ->   intermediate   ->    texture 
  //  (FILE_FMT)        (INT_FMT)          (TEX_FMT)  
  //
  // The user is allowed to choose TEX_FMT, but we have to choose a format for
  // the staging buffer (INT_FMT)
  // We should choose the format that minimizes the amount of conversion required
  // 

  // Choose the format of the file contents for the staging buffer
  // OpenGL should do the conversion
  gl::GLenum glchannels;
  switch (spec.nchannels) {
  case 1: glchannels = gl::RED; break;
  case 2: glchannels = gl::RG; break;
  case 3: glchannels = gl::RGB; break;
  case 4: glchannels = gl::RGBA; break;
	// TODO depth/stencil format? BRGA ordering?
  }

  gl::GLenum glcompfmt;
  switch (spec.format.basetype)
  {
  case TypeDesc::UINT8:
	  glcompfmt = gl::UNSIGNED_BYTE;
	  break;
  case TypeDesc::INT8:
	  glcompfmt = gl::BYTE;
	  break;
  case TypeDesc::UINT16:
	  glcompfmt = gl::UNSIGNED_SHORT;
	  break;
  case TypeDesc::INT16:
	  glcompfmt = gl::SHORT;
	  break;
  case TypeDesc::UINT:
	  glcompfmt = gl::UNSIGNED_INT;
	  break;
  case TypeDesc::INT:
	  glcompfmt = gl::INT;
	  break;
  case TypeDesc::HALF: 
	  glcompfmt = gl::HALF_FLOAT;
	  break;
  case TypeDesc::FLOAT: 
	  glcompfmt = gl::FLOAT;
	  break;
  case TypeDesc::UINT64: 
	  // fallthrough
  case TypeDesc::INT64:
	  // fallthrough
  case TypeDesc::DOUBLE:
	  // fallthrough
  default:
	  errorMessage("loadTexture: unsupported format {}", spec.format.c_str());
	  break;
  }

  // allocate the temporary buffer
  size_t bufsize = spec.width * spec.height * spec.format.basesize() * spec.nchannels;
  std::vector<unsigned char> buf( bufsize );
  // load data
  in->read_image(spec.format, buf.data());
  // create texture object
  Texture tex = Texture::create2D(targetFormat, spec.width, spec.height);
  // upload data
  gl::TextureSubImage2D(tex.object(), 0, 0, 0, spec.width, spec.height, glchannels, glcompfmt, buf.data());
  return tex;
}

void writeEXR(const char *path, const void *pixelData, int width, int height,
              ImageFormat inputFormat, ImageFormat targetFormat) 
{
	AG_DEBUG("Writing OpenEXR image {}x{} {}", width, height, getImageFormatInfo(inputFormat).name);
  /*if (inputFormat == ImageFormat::R16G16B16A16_SFLOAT) {
    Imf::RgbaChannels rgbaChannels = Imf::WRITE_RGBA;
    Imf::RgbaOutputFile outputFile{path, width, height, rgbaChannels};
    outputFile.setFrameBuffer((const Imf::Rgba *)pixelData, 1, width);
    outputFile.writePixels(height);
  } else {*/
    // TODO
    errorMessage("Not implemented");
}

void saveImageByPath(const char *path, const void *pixelData, int width,
                     int height, ImageFormat inputFormat,
                     ImageFormat targetFormat) {
  if (inputFormat != targetFormat) {
    errorMessage("saveImageByPath: for now, inputFormat and targetFormat must "
                 "be the same. Different target formats are ignored.");
    targetFormat = inputFormat;
  }

  if (inputFormat != ImageFormat::R8G8B8A8_SNORM &&
      inputFormat != ImageFormat::R8G8B8A8_UNORM &&
      inputFormat != ImageFormat::R8G8B8A8_SRGB &&
      inputFormat != ImageFormat::R16G16B16A16_SFLOAT &&
      inputFormat != ImageFormat::R16G16_SFLOAT) {
    errorMessage("saveImageByPath: unsupported input pixel format");
    return;
  }

  int nbcomp = 4;
  std::experimental::filesystem::path p{path};
  auto ext = p.extension().string();

  if (ext == ".exr") {
    writeEXR(path, pixelData, width, height, inputFormat, targetFormat);
  } else if (ext == ".png") {
    // assume lines are contiguous
    stbi_write_png(path, width, height, nbcomp, pixelData, 0);
  } else if (ext == ".bmp") {
    stbi_write_bmp(path, width, height, nbcomp, pixelData);
  } else if (ext == ".tga") {
    stbi_write_tga(path, width, height, nbcomp, pixelData);
  } else {
    errorMessage("saveImageByPath: unsupported target file format");
  }
}

void saveTexture(const char *path, Texture &texture,
                 ImageFormat targetFormat) {
  // Allocate buffer for readback
  gl::GLenum extFormat;
  gl::GLenum components;
  int size;

  switch (texture.format()) {
  case ImageFormat::R16G16B16A16_SFLOAT:
    extFormat = gl::HALF_FLOAT;
    components = gl::RGBA;
    size = 8;
    break;
  case ImageFormat::R16G16_SFLOAT:
    extFormat = gl::HALF_FLOAT;
    components = gl::RG;
    size = 4;
    break;
  case ImageFormat::R8G8B8A8_SNORM:
    extFormat = gl::UNSIGNED_INT_8_8_8_8_REV;
    components = gl::RGBA;
    size = 4;
    break;
  case ImageFormat::R8G8B8A8_UNORM:
    extFormat = gl::UNSIGNED_INT_8_8_8_8_REV;
    components = gl::RGBA;
    size = 4;
    break;
  case ImageFormat::R8G8B8A8_SRGB:
    extFormat = gl::UNSIGNED_INT_8_8_8_8_REV;
    components = gl::RGBA;
    size = 4;
    break;
  default:
    errorMessage("saveTexture: texture format not supported ({})",
                 getImageFormatInfo(texture.format()).name);
    return;
  }

  std::vector<uint8_t> data;
  auto bufSize = texture.width() * texture.height() * size;
  data.resize(bufSize);
  gl::GetTextureImage(texture.object(), 0, extFormat, components, bufSize,
                    data.data());
  saveImageByPath(path, data.data(), texture.width(), texture.height(),
                  texture.format(), texture.format());
}

} // namespace ag