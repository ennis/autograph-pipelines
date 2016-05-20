#pragma once
#include "image_impl.hpp"

#include "buffer_node.hpp"
#include "compute_node.hpp"
#include "shader_resource.hpp"

/////////////////////////////////////////////////////
// binders
class image;
struct buffer;

struct bind_resource_context {
  unsigned vertex_buffer_index = 0;
  unsigned uniform_buffer_index = 0;
  unsigned texture_index = 0;
  unsigned image_index = 0;
  unsigned sampler_index = 0;
};

struct sampled_image {
  const image &img;
};

struct storage_image {
  const image &img;
};

void bind_shader_resource(bind_resource_context &context, shader_resources &res,
                          image &img);
void bind_shader_resource(bind_resource_context &context, shader_resources &res,
                          buffer &buf);

template <typename T>
void bind_shader_resource(bind_resource_context &context, shader_resources &res,
                          const T &v) {
  auto buf = buffer{buffer_node::create(&v, sizeof(T), storage_type::host)};
  bind_shader_resource(context, res, buf);
}

/////////////////////////////////////////////////////
// Proxy for image_impl
class image {
public:
  static image clear_1d(image_format format, unsigned width,
                        const glm::vec4 &rgba);
  static image clear_2d(image_format format, unsigned width, unsigned height,
                        const glm::vec4 &rgba);
  static image clear_3d(image_format format, unsigned width, unsigned height,
                        unsigned depth, const glm::vec4 &rgba);

  image subimage(const rect_2d &rect);
  image cast(image_format format);
  image &set_storage_hint(storage_hint hint);
  image eager();

  //////////////////////////////////////////////
  // Operation of filter():
  // bind the input image to texture unit #0
  // bind the output image to image unit #0
  // bind other resources
  // return: image bound to img unit #0
  template <typename... Resources>
  image filter(const char *glsl, Resources &&... resources) {}

  template <typename... Resources>
  image filter(gl_compute_pipeline &pp, int local_size_x, int local_size_y,
               Resources &&... resources) {
    shader_resources res;
    shader_resource tex0;
    tex0.access = shader_resource_access::read;
    tex0.type = shader_resource_type::sampled_image;
    tex0.slot = 0;
    tex0.resource = impl_;
    res.push_back(std::move(tex0));
    shader_resource img0;
    auto img_out = std::make_shared<image_impl>(nullptr, impl_->desc_);
    img0.access = shader_resource_access::write;
    img0.type = shader_resource_type::storage_image;
    img0.slot = 0;
    img0.resource = img_out;

    // TODO
    // iterate over resources
    // image -> sampled_image
    // buffer -> uniform buffer
    // T -> uniform buffer

    bind_resource_context context;
    context.image_index = 1;
    context.texture_index = 1;
    for_each_in_tuple(std::forward_as_tuple(resources...),
                      [&](auto &&v) { bind_shader_resource(context, res, v); });

    auto n = compute_node::create(
        pp, compute_workspace::make_2d(glm::ivec2{(int)this->impl_->desc_.width,
                                                  (int)impl_->desc_.height},
                                       glm::ivec2{local_size_x, local_size_y}),
        std::move(res));

    img_out->pred_ = n.get();
    return image{std::move(img_out)};
  }

  // mark this image for rescheduling
  image &schedule();

  auto &name() const { return impl_->name_; }

  image &set_name(std::string name) {
    impl_->name_ = std::move(name);
    return *this;
  }

  // TODO: downsample/upsample
  // TODO: gen_mip_maps
  // TODO: ...

  image(std::shared_ptr<image_impl> impl) : impl_(impl) {}

  std::shared_ptr<image_impl> impl_;
};