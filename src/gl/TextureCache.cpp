#include "cppformat/format.h"
#include "TextureCache.hpp"
#include <iostream>

namespace ag {

std::shared_ptr<Texture> TextureCache::getTexture(const ImageDesc &desc) {
  auto it = cache_.find(desc);
  auto r = cache_.equal_range(desc);
  for (auto it = r.first; it != r.second; ++it) {
    if (it->second.unique()) {
      return it->second;
    }
  }

  fmt::print(std::clog, "Allocating temporary texture: {}x{}x{} {}\n",
             desc.width, desc.height, desc.depth,
             getImageFormatName(desc.format));
  return cache_.insert(std::make_pair(desc, std::make_shared<Texture>(desc)))
      ->second;
}
}