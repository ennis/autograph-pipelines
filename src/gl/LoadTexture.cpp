#include <stdexcept>
#include <fmt/format.h>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

/*
namespace ag 
{
Texture loadTexture(std::experimental::filesystem::path path,
                        ImageFormat tex_format) {
  auto pathstr = path.string();
  int x, y, comp;
  auto raw_data = stbi_load(pathstr.c_str(), &x, &y, &comp, 4);
  if (!raw_data)
    throw std::runtime_error{
        fmt::format("Missing or corrupt image file: {}", pathstr)};
  ImageDesc desc;
  desc.width = x;
  desc.height = y;
  desc.depth = 1;
  desc.format = tex_format;
  desc.numMipmaps = 1;
  // desc.storage_hint_ = storage_hint::device;
  desc.dimensions = ImageDimensions::Image2D;
  auto tex = Texture{desc};
  glTextureSubImage2D(tex.object(), 0, 0, 0, desc.width, desc.height,
                        GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, raw_data);
  return tex;
}

void saveTexture(std::experimental::filesystem::path path, Texture &tex) {
  fmt::print(std::cerr, "Saving texture {}...\n", path.string());
  auto pathstr = path.string();
  // read back pixels
  uint32_t *pixels = new uint32_t[tex.width() * tex.height()];
  glGetTextureImage(tex.object(), 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV,
                      4 * tex.width() * tex.height(), pixels);
  stbi_write_png(pathstr.c_str(), tex.width(), tex.height(), 4, pixels,
                 4 * tex.width());
  delete[] pixels;
}

}*/