#pragma once
#include "ResourceManager.h"
#include <autograph/Core/Cache.h>
#include <autograph/Core/Support/Optional.h>
#include <autograph/Gfx/Texture.h>
#include <memory>

namespace ag {
struct Image {
  ImageDesc desc;
  std::unique_ptr<uint8_t[]> data;
};

//
// Save a texture
//  void saveImageByPath(const char *path, const Image& img);

// Note: ideally, should take two formats:
// - one for the format of the input pixel data
// - one for the storage format
AG_ENGINE_API void saveImageByPath(const char *path, const void *pixelData,
                                   int width, int height,
                                   ImageFormat inputFormat,
                                   ImageFormat targetFormat);

// Note: ideally, should take two formats:
// - one for the format of the input pixel data
// - one for the storage format
AG_ENGINE_API void saveTexture(const char *path, Texture &texture,
                               ImageFormat targetFormat);

//
// Loads an image resource in main memory
// Will fail if the target format doesn't have
//		-  8-bit unorm/snorm/int/srgb components
//		OR 32-bit float components
AG_ENGINE_API Image loadImage(
    const char *id, ImageFormat targetFormat = ImageFormat::R8G8B8A8_SRGB);

//
// Loads a texture
AG_ENGINE_API Texture loadTexture(
    const char *id, ImageFormat targetFormat = ImageFormat::R8G8B8A8_SRGB);
} // namespace ag
