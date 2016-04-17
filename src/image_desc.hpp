#pragma once
#include "format.hpp"
#include "value.hpp"

enum class image_dimensions {
	img_1d,
	img_2d,
	img_3d,
	img_cube,
	img_1d_array,
	img_2d_array,
	img_cube_array
};

struct position_2d {
	position_2d() = default;
	position_2d(unsigned x_, unsigned y_) : x{ x_ }, y{ y_ }
	{}
	unsigned x = 0;
	unsigned y = 0;
};

struct extents_2d {
	extents_2d() = default;
	extents_2d(int w_, int h_) : width{ w_ }, height{ h_ } {}

	// -1 means whole image
	int width = -1;
	int height = -1;
};

struct rect_2d {
	rect_2d() = default;

	rect_2d(position_2d pos_, extents_2d size_ = extents_2d{}) : pos{ pos_ }, size{ size_ }
	{}

	rect_2d(unsigned x, unsigned y, int w, int h) : pos{ x, y }, size{w, h}
	{}

	position_2d pos;
	extents_2d size;
};

/////////////////////////////////////////////////////
// image_desc
struct image_desc {
	image_dimensions dimensions;
	image_format format;
	unsigned width;  // -1 if pipeline-dynamic
	unsigned height; // -1 if pipeline-dynamic
	unsigned depth;  // or array size
	unsigned num_mips;
	storage_hint storage_hint_;
};

