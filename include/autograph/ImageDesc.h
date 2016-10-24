#pragma once
#include <autograph/ImageFormat.h>

namespace ag {
enum class ImageDimensions {
  Image1D,
  Image2D,
  Image3D,
  ImageCubeMap,
  Image1DArray,
  Image2DArray,
  ImageCubeArray
};

struct ImageDesc {
  ImageDimensions dimensions;
  ImageFormat format;
  unsigned width;
  unsigned height;
  unsigned depth; // or array size, or number of faces
  unsigned numMipmaps;
};
}