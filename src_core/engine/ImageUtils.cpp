#include <ImfInputFile.h>
#include <ImfRgbaFile.h>
#include <ImfArray.h>
#include <autograph/engine/Application.h>
#include <autograph/engine/ImageUtils.h>
#include <autograph/support/Debug.h>
#include <experimental/filesystem>

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
                                                     int &comp, int req_cmp) {
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

Texture loadTexture(const char *id, ImageFormat targetFormat) {
  auto path = ResourceManager::getFilesystemPath(id);
  if (path.empty()) {
    throw std::runtime_error{"loadTexture: file not found"};
  }

  Texture tex;

  // inspect extension to see if we must load a EXR image
  if (fs::path{path}.extension() == ".exr") {
    // TODO support more target formats
    if (targetFormat == ImageFormat::R16G16B16A16_SFLOAT) {
      Imf::Array2D<Imf::Rgba> data;
      Imf::RgbaInputFile inputFile{path.c_str()};
      auto dw = inputFile.dataWindow();
      int width = dw.max.x - dw.min.x + 1;
      int height = dw.max.y - dw.min.y + 1;
      data.resizeErase(height, width);
      AG_DEBUG("Loading OpenEXR image {}x{}", width, height);
      inputFile.setFrameBuffer(&data[0][0] - dw.min.x - dw.min.y * width, 1,
                               width);
      inputFile.readPixels(
          dw.min.y, dw.max.y); // create target texture and upload our data
      tex = Texture::create2D(targetFormat, width, height);
      gl::TextureSubImage2D(tex.object(), 0, 0, 0, width, height, gl::RGBA16F,
                          gl::HALF_FLOAT, &data[0][0]);
    } else {
      errorMessage("Not implemented");
    }
    // TODO advanced loading
    // load with OpenEXR
    // Imf::Array2D<Imf::Rgba> data;
    // Imf::InputFile inputFile{ path.c_str() };

    // inputFile.
    // const Imf::ChannelList& channels = inputFile.header().channels();
    // for (Imf::ChannelList::ConstIterator i = channels.begin(); i !=
    // channels.end(); ++i) {
    //  const Imf::Channel& channel = i.channel();
    //}

    // Look for different channels depending on the target texture format
  } else {
    // load with stbimage
    int comp;
    int w, h;
    auto data = loadImageByPathRaw(path.c_str(), w, h, comp, 4);
    tex = Texture::create2D(targetFormat, w, h);
	gl::TextureSubImage2D(tex.object(), 0, 0, 0, w, h, gl::RGBA,
                        gl::UNSIGNED_INT_8_8_8_8_REV, data.get());
  }
  return tex;
}

void writeEXR(const char *path, const void *pixelData, int width, int height,
              ImageFormat inputFormat, ImageFormat targetFormat) 
{
	AG_DEBUG("Writing OpenEXR image {}x{} {}", width, height, getImageFormatInfo(inputFormat).name);
  if (inputFormat == ImageFormat::R16G16B16A16_SFLOAT) {
    Imf::RgbaChannels rgbaChannels = Imf::WRITE_RGBA;
    Imf::RgbaOutputFile outputFile{path, width, height, rgbaChannels};
    outputFile.setFrameBuffer((const Imf::Rgba *)pixelData, 1, width);
    outputFile.writePixels(height);
  } else {
    // TODO
    errorMessage("Not implemented");
  }
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