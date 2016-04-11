#include "cppformat/format.h"
#include "texture_cache.hpp"
#include <iostream>

std::shared_ptr<gl_texture>
texture_cache::get_temporary_texture(image_desc desc) {
  auto it = cache_.find(desc);
  if (it != cache_.end()) {
    if (it->second.unique()) {
      return it->second;
    }
  }

  fmt::print(std::clog, "Allocating temporary texture: {}x{}x{} {}\n",
             desc.width, desc.height, desc.depth,
             get_image_format_name(desc.format));
  return cache_.insert(std::make_pair(desc, std::make_shared<gl_texture>(desc)))
      .first->second;
}
