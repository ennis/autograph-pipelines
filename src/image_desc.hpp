#pragma once
#include "format.hpp"
#include "value.hpp"
#include "rect.hpp"

enum class image_dimensions {
  img_1d,
  img_2d,
  img_3d,
  img_cube,
  img_1d_array,
  img_2d_array,
  img_cube_array
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
