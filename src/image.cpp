#include "cast_node.hpp"
#include "clear_node.hpp"
#include "image.hpp"
#include "subimage_node.hpp"

namespace {
image_desc nd_image_desc(image_format format, image_dimensions dims,
                         unsigned width, unsigned height, unsigned depth) {
  image_desc d;
  d.dimensions = dims;
  d.format = format;
  d.width = width;
  d.height = height;
  d.depth = depth;
  d.num_mips = 1;
  d.storage_hint_ = storage_hint::automatic;
  return d;
}
}
