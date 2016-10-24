#pragma once
#include <autograph/Config.h>
#include <array>
#include <cstdint>
#include <cstdlib>

namespace ag {
enum class ImageFormat {
  RGBA32_Float = 0,
  RGBA16_Float,
  RG32_Float,
  R32_Float,
  R8_Unorm,
  RGBA8_Unorm,
  RGBA8_Snorm,
  RGB_11_11_10_Float_Packed,
  Depth32_Float,
  Max
};

struct ImageFormatInfo {
  const char* name;
  uint32_t size;
  uint32_t numChannels;
};

AG_API const ImageFormatInfo &getImageFormatInfo(ImageFormat fmt);
}
