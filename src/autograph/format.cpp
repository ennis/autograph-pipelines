#include "format.hpp"

namespace ag {
/*
enum class image_format {
  rgba32_float = 0,
  rgba16_float,
  rg32_float,
  r32_float,
  r8_unorm,
  rgba8_unorm,
  rgba8_snorm,
  rgb_11_11_10_float_packed,
  depth32_float,
  max
};*/

std::array<image_format_info, static_cast<std::size_t>(image_format::max)>
    format_infos = {{
        {16, 4}, // rgba32_float
        {8, 4},  // rgba16_float
        {8, 2},  // rg32_float
        {4, 1},  // r32_float
        {1, 1},  // r8_unorm
        {4, 4},  // rgba8_unorm
        {4, 4},  // rgba8_snorm
        {4, 3},  // rgb_11_11_10_float_packed
        {4, 1}   // depth32_float
    }};

std::array<const char *, static_cast<std::size_t>(image_format::max)>
    format_names = {{"rgba32_float", "rgba16_float", "rg32_float", "r32_float",
                     "r8_unorm", "rgba8_unorm", "rgba8_snorm",
                     "rgb_11_11_10_float_packed", "depth32_float"}};

const image_format_info &get_image_format_info(image_format fmt) {
  return format_infos[static_cast<std::size_t>(fmt)];
}

const char *get_image_format_name(image_format fmt) {
  return format_names[static_cast<std::size_t>(fmt)];
}
}