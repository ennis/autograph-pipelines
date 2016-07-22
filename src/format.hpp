#pragma once

#include <cstdlib>
#include <cstdint>
#include <array>

#define META(V) [[cxxr::meta(V)]]

struct DisplayName {
	const char* str;
};

enum class[[cxxr::reflect]] META(DisplayName{ "Image format" }) image_format{
	rgba32_float META(DisplayName("RGBA32_FLOAT")) = 0,
	rgba16_float META(DisplayName("RGBA16_FLOAT")),
	rg32_float META(DisplayName("RG32_FLOAT")),
	r32_float META(DisplayName("R32_FLOAT")),
	rgba8_unorm META(DisplayName("RGBA8_UNORM")),
	rgba8_snorm META(DisplayName("RGBA8_DNORM")),
	rgb_11_11_10_float_packed META(DisplayName("RGB_11_11_10_FLOAT_PACKED")),
	depth32_float META(DisplayName("DEPTH32_FLOAT")),
	max
};

struct[[cxxr::reflect]] image_format_info
{
	uint32_t size;
	uint32_t num_channels;
};

const image_format_info& get_image_format_info(image_format fmt);
const char* get_image_format_name(image_format fmt);
