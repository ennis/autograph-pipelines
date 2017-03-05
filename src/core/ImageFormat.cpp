#include <autograph/ImageFormat.h>
#include <array>

namespace ag {
/*
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
  Max

  max
};*/

std::array<ImageFormatInfo, static_cast<std::size_t>(ImageFormat::Max)>
    format_infos = {{
        {"R32G32B32A32_SFLOAT", 16, 4},             // RGBA32_Float
        {"R16G16B16A16_SFLOAT", 8, 4},              // RGBA16_Float
        {"R32G32_SFLOAT", 8, 2},                // RG32_Float
        {"R32_SFLOAT", 4, 1},                 // R32_Float
        {"R8_UNORM", 1, 1},                  // R8_Unorm
        {"R8G8B8A8_UNORM", 4, 4},               // rgba8_unorm
        {"R8G8B8A8_SNORM", 4, 4},               // rgba8_snorm
        {"B10G11R11_UFLOAT_PACK32", 4, 3}, // rgb_11_11_10_float_packed
        {"D32_SFLOAT", 4, 1},              // depth32_float
        {"A2R10G10B10_UNORM_PACK32", 4, 4},
{"A2R10G10B10_SNORM_PACK32", 4, 4},
{ "R8_SRGB", 1, 1 },
{ "R8G8_SRGB", 2, 2 },
{ "R8G8B8_SRGB", 3, 3 },
{ "R8G8B8A8_SRGB", 4, 4 },
{ "R32G32B32A32_UINT", 16, 4 },
{ "R16G16_SFLOAT", 4, 2 },
{ "R16G16_SINT", 4, 2 },
    }};

AG_API const ImageFormatInfo &getImageFormatInfo(ImageFormat fmt) {
  return format_infos[static_cast<std::size_t>(fmt)];
}
}
