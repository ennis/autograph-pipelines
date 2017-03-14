#pragma once
#include <autograph/Config.h>
#include <cstdint>
#include <cstdlib>

namespace ag {
enum class AG_REFLECT ImageFormat {
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
  R8_SRGB,
  R8G8_SRGB,
  R8G8B8_SRGB,
  R8G8B8A8_SRGB,
  R32G32B32A32_UINT,
  R16G16_SFLOAT,
  R16G16_SINT,
  Max
};

struct ImageFormatInfo {
  const char* name;
  uint32_t size;
  uint32_t numChannels;
};

AG_CORE_API const ImageFormatInfo &getImageFormatInfo(ImageFormat fmt);
}
