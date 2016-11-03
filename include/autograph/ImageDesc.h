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
  int width;
  int height;
  int depth; // or array size, or number of faces
  int numMipmaps;
};
}