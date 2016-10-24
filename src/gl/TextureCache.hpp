#pragma once
#include <autograph/gl/Texture.h>
#include <memory>
#include <unordered_map>
#include <type_traits>

namespace ag {
namespace gl {
//////////////////////////////////////////////////////
// Texture cache
class TextureCache {
public:
  struct image_desc_equal {
    constexpr bool operator()(const ImageDesc &a, const ImageDesc &b) const {
      return (a.width == b.width) && (a.height == b.height) &&
             (a.depth == b.depth) && (a.format == b.format) &&
             (a.numMipmaps == b.numMipmaps) && (a.dimensions == b.dimensions);
    }
  };
  struct image_desc_hash {
    template <typename T>
    constexpr size_t hash_accum(size_t a, const T &obj) const {
      return a ^ (std::hash<T>{}(obj) << 1);
    }

    size_t operator()(const ImageDesc &d) const {
      size_t h = 0;
      h = hash_accum(h, d.width);
      h = hash_accum(h, d.height);
      h = hash_accum(h, d.depth);
      h = hash_accum(
          h, static_cast<std::underlying_type_t<ImageFormat>>(d.format));
      h = hash_accum(h, d.numMipmaps);
      h = hash_accum(h, static_cast<std::underlying_type_t<ImageDimensions>>(
                            d.dimensions));
      return h;
    }
  };

  // return a temporary texture from the cache
  // not thread-safe, this won't prevent two threads
  // from writing into the same temporary texture from
  // two different command buffers
  std::shared_ptr<Texture> getTexture(const ImageDesc &desc);

private:
  std::unordered_multimap<ImageDesc, std::shared_ptr<Texture>, image_desc_hash,
                          image_desc_equal>
      cache_;
};
}
}