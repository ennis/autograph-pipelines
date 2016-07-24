#pragma once
#include <unordered_map>
#include <memory>
#include "texture.hpp"

namespace ag 
{
//////////////////////////////////////////////////////
// Texture cache
class texture_cache {
public:
  struct image_desc_equal {
    constexpr bool operator()(const image_desc &a, const image_desc &b) const {
      return (a.width == b.width) && (a.height == b.height) &&
             (a.depth == b.depth) && (a.format == b.format) &&
             (a.num_mips == b.num_mips) && (a.dimensions == b.dimensions);
    }
  };
  struct image_desc_hash {
    template <typename T>
    constexpr size_t hash_accum(size_t a, const T &obj) const {
      return a ^ (std::hash<T>{}(obj) << 1);
    }

    size_t operator()(const image_desc &d) const {
      size_t h = 0;
      h = hash_accum(h, d.width);
      h = hash_accum(h, d.height);
      h = hash_accum(h, d.depth);
      h = hash_accum(h, static_cast<std::underlying_type_t<image_format>>(d.format));
      h = hash_accum(h, d.num_mips);
      h = hash_accum(h, static_cast<std::underlying_type_t<image_dimensions>>(d.dimensions));
      return h;
    }
  };

  // return a temporary texture from the cache
  // not thread-safe, this won't prevent two threads
  // from writing into the same temporary texture from 
  // two different command buffers
  std::shared_ptr<texture> get_texture(const image_desc& desc);

private:
  std::unordered_multimap<image_desc, std::shared_ptr<texture>, image_desc_hash,
                     image_desc_equal>
      cache_;
};

}