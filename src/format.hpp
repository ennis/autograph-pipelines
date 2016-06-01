#pragma once

#include <cstdlib>
#include <cstdint>
#include <array>

enum class image_format {
	rgba32_float = 0,
	rgba16_float,
	rg32_float,
	r32_float,
	rgba8_unorm,
	rgba8_snorm,
	rgb_11_11_10_float_packed,
	max
};

struct image_format_info
{
	uint32_t size;
	uint32_t num_channels;
};

const image_format_info& get_image_format_info(image_format fmt);
const char* get_image_format_name(image_format fmt);