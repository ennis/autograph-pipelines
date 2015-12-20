#ifndef SURFACE_HPP
#define SURFACE_HPP

#include <PixelType.hpp>

namespace ag
{
	// A type representing a 2D image in a flow graph
	// Surfaces can be backed by a texture, a renderbuffer, or the default framebuffer
	template <typename TPixel>
	struct Surface
	{
		static_assert(PixelTypeTraits<TPixel>::IsPixelType, "Surface<TPixel>: TPixel does not implement PixelTypeTraits");

		int width;
		int height;
	};
}

#endif
