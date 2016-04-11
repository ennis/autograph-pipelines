#ifndef SURFACE_HPP
#define SURFACE_HPP

#include "pixel_format.hpp"
#include "gl_types.hpp"

namespace ag {
template <typename TDepth, typename... TColors> 
	struct Surface {
  GLuint obj;
  unsigned width;
  unsigned height;
};
}

#endif
