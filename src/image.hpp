#pragma once
#include "image_impl.hpp"
#include "buffer_node.hpp"
#include "compute_node.hpp"
#include "shader_resource.hpp"

/////////////////////////////////////////////////////
// An image pyramid
// Proxy for image_impl
class image {
public:
  static image clear_1d(image_format format, unsigned width,
                        const glm::vec4 &rgba);
  static image clear_2d(image_format format, unsigned width, unsigned height,
                        const glm::vec4 &rgba);
  static image clear_3d(image_format format, unsigned width, unsigned height,
                        unsigned depth, const glm::vec4 &rgba);

  // mark this image for rescheduling
  image &schedule();

  auto &name() const { return impl_->name_; }

  image &set_name(std::string name) {
    impl_->name_ = std::move(name);
    return *this;
  }

  image subimage(const rect_2d &rect);
  image cast(image_format format);
  image &set_storage_hint(storage_hint hint);

  //////////////////////////////////////////////
  // Operation of filter():
  // bind the input image to texture unit #0
  // bind the output image to image unit #0
  // bind other resources
  // return: image bound to img unit #0
  template <typename... Resources>
  image filter(const glsl_snippet& snip, Resources &&... resources) {}

  template <typename... Resources>
  image filter(gl_compute_pipeline &pp, int local_size_x, int local_size_y,
               Resources &&... resources) {
    /*shader_resources res;
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
    return image{std::move(img_out)};*/
  }

  // Fullscreen fragment shader pass
  template <typename... Resources>
  image apply_frag(gl_graphics_pipeline &pp, Resources&&... resources)
  {}

  // same but with a GLSL snippet
  template <typename... Resources>
  image apply_frag(const glsl_snippet &snip, Resources&&... resources)
  {
      // TODO: get pipeline from GLSL snippet compiler cache
      // call apply_frag
  }

  // downsample the image (x2)
  template <typename... Resources>
  image downsample(const glsl_snippet& snip, Resources&&... resources)
  {
    // TODO
  }

  // regenerate the mip-maps
  // will evaluate predecessors if necessary
  image &generate_mip_maps()
  {
      // TODO
  }

  // regen the mip-maps with a custom GLSL filter
  // will evaluate predecessors if necessary
  template <typename... Resources>
  image &generate_mip_maps(const glsl_snippet& snip, Resources&&... resources)
  {
      // TODO
  }

  //img.generate_mip_maps("void apply(vec4 a, vec4 b, vec4 c, vec4 d) { .... }");


  // TODO: downsample/upsample
  // TODO: gen_mip_maps
  // TODO: ...

  image(std::shared_ptr<image_impl> impl) : impl_(impl) {}

  std::shared_ptr<image_impl> impl_;
};
