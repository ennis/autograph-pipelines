#include "compute_node.hpp"
#include <iostream>
#include <cppformat/format.h>

#include "gl_texture.hpp"
#include "gl_buffer.hpp"

void compute_node::traverse(traversal_visitor &v) {
  for (auto &r : res) {
    if (not_empty(r.access & shader_resource_access::write)) {
      v.visit_value(*r.resource);
    }
  }
}

void compute_node::allocate_resources(allocation_context &ctx) {
  if (alloc_)
    return;
  for (const auto &r : res) {
    if (r.access == shader_resource_access::read) {
      if (r.resource->pred_) {
      	r.resource->pred_->allocate_resources(ctx);
      }
    } else {
      if (r.type == shader_resource_type::storage_image) {
        auto &img = static_cast<image_impl &>(*r.resource);
        fmt::print(std::clog, "allocating texture {}x{}x{} format {}",
                   img.desc_.width, img.desc_.height, img.desc.depth_,
                   get_image_format_name(img.format));
        auto gltex = std::make_unique<gl_texture>(img.desc_);
        // TODO target img may prefer CPU-accessible memory
        img.stype = storage_type::device;
        img.storage.device_tex = gltex.get();
        texres_.emplace_back(gltex);
      }
      else if (r.type == shader_resource_type::storage_buffer) {
        auto& buf = static_cast<buffer_impl&>(*r.resource);
        fmt::print(std::clog, "allocating buffer size {}", buf.size);
        
      } 
    }
  }
}

std::shared_ptr<compute_node>
compute_node::create(compute_pipeline_program &prog,
                     const compute_workspace &ws, shader_resources res) {
  auto n = std::make_shared<compute_node>();
  n->ws = ws;
  n->res = std::move(res);
  n->pp = &prog;
  return std::move(n);
}
