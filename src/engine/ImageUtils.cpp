#include <autograph/engine/Application.h>
#include <autograph/engine/ImageUtils.h>
#include <autograph/support/Debug.h>
#include <experimental/filesystem>

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

AG_API Image loadImageByPath(const char *path, ImageFormat targetFormat) {
  // TODO DDS loader
  // TODO mip maps, subresources
  // TODO faces
  // TODO FP formats
  // TODO conversions
  Image img;
  int comp;
  // TODO filter out unsupported formats
  img.data = loadImageByPathRaw(path, img.desc.width, img.desc.height, comp, 4);
  img.desc.numMipmaps = 1;
  img.desc.dimensions = ImageDimensions::Image2D;
  img.desc.depth = 1;
  img.desc.format = targetFormat;
  return img;
}

AG_API gl::Texture loadTextureByPath(const char *path,
                                     ImageFormat targetFormat) {
  int comp;
  int w, h;
  auto data = loadImageByPathRaw(path, w, h, comp, 4);
  auto tex = gl::Texture::create2D(targetFormat, w, h);
  glTextureSubImage2D(tex.object(), 0, 0, 0, w, h, GL_RGBA,
                      GL_UNSIGNED_INT_8_8_8_8_REV, data.get());
  return tex;
}

static const char *allowedImageExtensions[] = {".png", ".jpeg", ".bmp", ".jpg",
                                               ".tga"};

AG_API gl::Texture loadTexture(const char *id, ImageFormat targetFormat) {
  return loadTextureByPath(findResourceFile(id, allowedImageExtensions).c_str(),
                           targetFormat);
}

AG_API Image loadImage(const char *id, ImageFormat targetFormat) {
  return loadImageByPath(findResourceFile(id, allowedImageExtensions).c_str(),
                         targetFormat);
}

AG_API void saveImageByPath(const char *path, const void *pixelData, int width, int height, ImageFormat format) {
  if (format != ImageFormat::R8G8B8A8_SNORM &&
      format != ImageFormat::R8G8B8A8_UNORM &&
      format != ImageFormat::R8G8B8A8_SRGB) {
    throw std::runtime_error{"Unsupported pixel format"};
  }

  int nbcomp = 4;
  std::experimental::filesystem::path p{path};
  auto ext = p.extension().string();

  if (ext == ".png") {
    // assume lines are contiguous
    stbi_write_png(path, width, height, nbcomp, pixelData, 0);
  } else if (ext == ".bmp") {
    stbi_write_bmp(path, width, height, nbcomp, pixelData);
  } else if (ext == ".tga") {
    stbi_write_tga(path, width, height, nbcomp, pixelData);
  } else {
    throw std::runtime_error{"Unsupported target file format"};
  }
}
}