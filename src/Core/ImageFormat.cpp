#include <array>
#include <autograph/Core/ImageFormat.h>

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
    format_infos = {
        {{"R32G32B32A32_SFLOAT", 16, 4},    // RGBA32_Float
         {"R16G16B16A16_SFLOAT", 8, 4},     // RGBA16_Float
         {"R32G32_SFLOAT", 8, 2},           // RG32_Float
         {"R32_SFLOAT", 4, 1},              // R32_Float
         {"R8_UNORM", 1, 1},                // R8_Unorm
         {"R8G8B8A8_UNORM", 4, 4},          // rgba8_unorm
         {"R8G8B8A8_SNORM", 4, 4},          // rgba8_snorm
         {"B10G11R11_UFLOAT_PACK32", 4, 3}, // rgb_11_11_10_float_packed
         {"D32_SFLOAT", 4, 1},              // depth32_float
         {"A2R10G10B10_UNORM_PACK32", 4, 4},
         {"A2R10G10B10_SNORM_PACK32", 4, 4},
         {"R8_SRGB", 1, 1},
         {"R8G8_SRGB", 2, 2},
         {"R8G8B8_SRGB", 3, 3},
         {"R8G8B8A8_SRGB", 4, 4},
         {"R32G32B32A32_UINT", 16, 4},
         {"R16G16_SFLOAT", 4, 2},
         {"R16G16_SINT", 4, 2},
         {"BC1_RGB_UNORM_BLOCK", 0, 0},
         {"BC1_RGB_SRGB_BLOCK", 0, 0},
         {"BC1_RGBA_UNORM_BLOCK", 0, 0},
         {"BC1_RGBA_SRGB_BLOCK", 0, 0},
         {"BC2_UNORM_BLOCK", 0, 0},
         {"BC2_SRGB_BLOCK", 0, 0},
         {"BC3_UNORM_BLOCK", 0, 0},
         {"BC3_SRGB_BLOCK", 0, 0},
         {"BC4_UNORM_BLOCK", 0, 0},
         {"BC4_SNORM_BLOCK", 0, 0},
         {"BC5_UNORM_BLOCK", 0, 0},
         {"BC5_SNORM_BLOCK", 0, 0},
         {"BC6H_UFLOAT_BLOCK", 0, 0},
         {"BC6H_SFLOAT_BLOCK", 0, 0},
         {"BC7_UNORM_BLOCK", 0, 0},
         {"BC7_SRGB_BLOCK", 0, 0},
         {"ETC2_R8G8B8_UNORM_BLOCK", 0, 0},
         {"ETC2_R8G8B8_SRGB_BLOCK", 0, 0},
         {"ETC2_R8G8B8A1_UNORM_BLOCK", 0, 0},
         {"ETC2_R8G8B8A1_SRGB_BLOCK", 0, 0},
         {"ETC2_R8G8B8A8_UNORM_BLOCK", 0, 0},
         {"ETC2_R8G8B8A8_SRGB_BLOCK", 0, 0},
         {"EAC_R11_UNORM_BLOCK", 0, 0},
         {"EAC_R11_SNORM_BLOCK", 0, 0},
         {"EAC_R11G11_UNORM_BLOCK", 0, 0},
         {"EAC_R11G11_SNORM_BLOCK", 0, 0},
         {"ASTC_4x4_UNORM_BLOCK", 0, 0},
         {"ASTC_4x4_SRGB_BLOCK", 0, 0},
         {"ASTC_5x4_UNORM_BLOCK", 0, 0},
         {"ASTC_5x4_SRGB_BLOCK", 0, 0},
         {"ASTC_5x5_UNORM_BLOCK", 0, 0},
         {"ASTC_5x5_SRGB_BLOCK", 0, 0},
         {"ASTC_6x5_UNORM_BLOCK", 0, 0},
         {"ASTC_6x5_SRGB_BLOCK", 0, 0},
         {"ASTC_6x6_UNORM_BLOCK", 0, 0},
         {"ASTC_6x6_SRGB_BLOCK", 0, 0},
         {"ASTC_8x5_UNORM_BLOCK", 0, 0},
         {"ASTC_8x5_SRGB_BLOCK", 0, 0},
         {"ASTC_8x6_UNORM_BLOCK", 0, 0},
         {"ASTC_8x6_SRGB_BLOCK", 0, 0},
         {"ASTC_8x8_UNORM_BLOCK", 0, 0},
         {"ASTC_8x8_SRGB_BLOCK", 0, 0},
         {"ASTC_10x5_UNORM_BLOCK", 0, 0},
         {"ASTC_10x5_SRGB_BLOCK", 0, 0},
         {"ASTC_10x6_UNORM_BLOCK", 0, 0},
         {"ASTC_10x6_SRGB_BLOCK", 0, 0},
         {"ASTC_10x8_UNORM_BLOCK", 0, 0},
         {"ASTC_10x8_SRGB_BLOCK", 0, 0},
         {"ASTC_10x10_UNORM_BLOCK", 0, 0},
         {"ASTC_10x10_SRGB_BLOCK", 0, 0},
         {"ASTC_12x10_UNORM_BLOCK", 0, 0},
         {"ASTC_12x10_SRGB_BLOCK", 0, 0},
         {"ASTC_12x12_UNORM_BLOCK", 0, 0},
         {"ASTC_12x12_SRGB_BLOCK", 0, 0}}};

const ImageFormatInfo &getImageFormatInfo(ImageFormat fmt) {
  return format_infos[static_cast<std::size_t>(fmt)];
}
}
