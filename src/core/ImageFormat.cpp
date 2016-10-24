#include <autograph/ImageFormat.h>

namespace ag {
/*
enum class ImageFormat {
  RGBA32_Float = 0,
  RGBA16_Float,
  RG32_Float,
  R32_Float,
  R8_Unorm,
  rgba8_unorm,
  rgba8_snorm,
  rgb_11_11_10_float_packed,
  depth32_float,
  max
};*/

std::array<ImageFormatInfo, static_cast<std::size_t>(ImageFormat::Max)>
    format_infos = {{
        {"RGBA32_Float", 16, 4},             // RGBA32_Float
        {"RGBA16_Float", 8, 4},              // RGBA16_Float
        {"RG32_Float", 8, 2},                // RG32_Float
        {"R32_Float", 4, 1},                 // R32_Float
        {"R8_Unorm", 1, 1},                  // R8_Unorm
        {"RGBA8_unorm", 4, 4},               // rgba8_unorm
        {"RGBA8_snorm", 4, 4},               // rgba8_snorm
        {"RGB_11_11_10_Float_Packed", 4, 3}, // rgb_11_11_10_float_packed
        {"Depth32_Float", 4, 1}              // depth32_float
    }};

AG_API const ImageFormatInfo &getImageFormatInfo(ImageFormat fmt) {
  return format_infos[static_cast<std::size_t>(fmt)];
}
}