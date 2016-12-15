#pragma once
#include <autograph/Config.h>
#include <array>
#include <cstdint>
#include <cstdlib>

namespace ag {
enum class ImageFormat {
  R32G32B32A32_SFLOAT = 0,
  R16G16B16A16_SFLOAT,
  R32G32_SFLOAT,
  R32_SFLOAT,
  R8_UNORM,
  R8G8B8A8_UNORM,
  R8G8B8A8_SNORM,
  B10G11R11_UFLOAT_PACK32,
  D32_SFLOAT,
  A2R10G10B10_UNORM_PACK32,
  A2R10G10B10_SNORM_PACK32,
  Max
};

struct ImageFormatInfo {
  const char* name;
  uint32_t size;
  uint32_t numChannels;
};

AG_API const ImageFormatInfo &getImageFormatInfo(ImageFormat fmt);
}
