#pragma once
#include <autograph/Core/ImageFormat.h>
#include <autograph/Gfx/gl_core_4_5.h>

namespace ag {
/// Structure containing information about the OpenGL internal format
/// corresponding to an 'ImageFormat'
struct GLFormatInfo {
  gl::GLenum internal_fmt; //< Corresponding internal format
  gl::GLenum external_fmt; //< Preferred external format for uploads/reads
  gl::GLenum type;         //< Preferred element type for uploads/reads
  int num_comp;            //< number of components (channels) (TODO redundant)
  int size;                //< Size of one pixel in bytes
};

/// Returns information about the OpenGL internal format corresponding to the
/// specified ImageFormat
const GLFormatInfo &getGLImageFormatInfo(ImageFormat fmt);

} // namespace ag