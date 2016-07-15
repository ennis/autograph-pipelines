#pragma once
#include "image_impl.hpp"
#include "shader_resource.hpp"

class[[cxxr::reflect]] image {
public:
  auto &name() const { return impl_->name_; }

  image &set_name(std::string name) {
    impl_->name_ = std::move(name);
    return *this;
  }

  image(std::shared_ptr<image_impl> impl) : impl_(impl) {}

  image create(glm::ivec2 size_2d, image_format fmt, node& n) {
	  auto ptr = std::make_shared<image_impl>();
	  ptr->desc_.dimensions = image_dimensions::img_2d;
	  ptr->desc_.width = size_2d.x;
	  ptr->desc_.height = size_2d.y;
	  ptr->desc_.depth = 1;
	  ptr->desc_.format = fmt;
	  ptr->desc_.num_mips = 0;
	  ptr->pred_ = &n;
	  return image{ std::move(ptr) };
  }

  std::shared_ptr<image_impl> impl_;
};
