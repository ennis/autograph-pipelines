#include "format.hpp"

/*enum class image_format {
        rgba32_float = 0,
        rgba16_float,
        rg32_float,
        r32_float,
        rgba8_unorm,
        rgba8_snorm,
        rgb_11_11_10_float_packed,
		depth32_float,
        max
};*/

std::array<image_format_info, static_cast<std::size_t>(image_format::max)>
    format_infos = {{
        {16, 4}, {8, 4}, {8, 2}, {4, 1}, {4, 4}, {4, 4}, {4, 3}, { 4, 1 },
    }};

std::array<const char *, static_cast<std::size_t>(image_format::max)>
    format_names = {{
        "rgba32_float", "rgba16_float", "rg32_float", "r32_float",
        "rgba8_unorm", "rgba8_snorm", "rgb_11_11_10_float_packed", "depth32_float"
    }};

const image_format_info &get_image_format_info(image_format fmt) {
  return format_infos[static_cast<std::size_t>(fmt)];
}

const char *get_image_format_name(image_format fmt) {
  return format_names[static_cast<std::size_t>(fmt)];
}
