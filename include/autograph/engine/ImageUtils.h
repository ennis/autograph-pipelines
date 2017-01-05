#pragma once
#include <autograph/gl/Texture.h>
#include <memory>
#include <autograph/support/Optional.h>
#include "ResourceManager.h"

namespace ag
{
	struct Image
	{
		ImageDesc desc;
		std::unique_ptr<uint8_t[]> data;
	};
	
	//
	// Loads an image resource in main memory
	// Will fail if the target format doesn't have 
	//		-  8-bit unorm/snorm/int/srgb components
	//		OR 32-bit float components
	AG_API Image loadImageByPath(const char* path, ImageFormat targetFormat = ImageFormat::R8G8B8A8_SRGB);

	//
	// Loads a texture
	AG_API gl::Texture loadTextureByPath(const char* path, ImageFormat targetFormat = ImageFormat::R8G8B8A8_SRGB);

	// Helpers
	AG_API gl::Texture loadTexture(const char* id, ImageFormat targetFormat = ImageFormat::R8G8B8A8_SRGB);
	AG_API Image loadImage(const char* id, ImageFormat targetFormat = ImageFormat::R8G8B8A8_SRGB);
}