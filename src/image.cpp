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

image image::clear_1d(image_format format, unsigned width,
                      const glm::vec4 &rgba) {
  return image{clear_node::create(
      nd_image_desc(format, image_dimensions::img_1d, width, 1, 1), rgba)};
}

image image::clear_2d(image_format format, unsigned width, unsigned height,
                      const glm::vec4 &rgba) {
  return image{clear_node::create(
      nd_image_desc(format, image_dimensions::img_2d, width, height, 1), rgba)};
}

image image::clear_3d(image_format format, unsigned width, unsigned height,
                      unsigned depth, const glm::vec4 &rgba) {
  return image{clear_node::create(
      nd_image_desc(format, image_dimensions::img_3d, width, height, depth),
      rgba)};
}

image image::subimage(const rect_2d &rect) {
  return image{subimage_2d_node::create(impl_, rect)};
}

image image::cast(image_format format) {
  return image{cast_node::create(impl_, format)};
}

void bind_shader_resource(bind_resource_context &context, shader_resources &res,
                          image &img) {}

void bind_shader_resource(bind_resource_context &context, shader_resources &res,
                          buffer &buf) {
  res.emplace_back(shader_resource{
      std::move(buf.impl_), shader_resource_type::uniform_buffer,
      shader_resource_access::read, context.uniform_buffer_index++});
}
