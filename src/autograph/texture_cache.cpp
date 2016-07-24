#include "cppformat/format.h"
#include "texture_cache.hpp"
#include <iostream>

namespace ag {

std::shared_ptr<texture> texture_cache::get_texture(const image_desc &desc) {
  auto it = cache_.find(desc);
  auto r = cache_.equal_range(desc);
  for (auto it = r.first; it != r.second; ++it) {
    if (it->second.unique()) {
      return it->second;
    }
  }

  fmt::print(std::clog, "Allocating temporary texture: {}x{}x{} {}\n",
             desc.width, desc.height, desc.depth,
             get_image_format_name(desc.format));
  return cache_.insert(std::make_pair(desc, std::make_shared<texture>(desc)))
      ->second;
}
}