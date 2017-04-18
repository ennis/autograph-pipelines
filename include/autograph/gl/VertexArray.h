#pragma once
#include <autograph/Config.h>
#include <autograph/support/Span.h>
#include <autograph/gl/GLHandle.h>

namespace ag {

struct VertexArrayDeleter {
	static constexpr gl::GLenum objectType = gl::VERTEX_ARRAY;
  void operator()(gl::GLuint obj) { gl::DeleteVertexArrays(1, &obj); }
};

struct AG_REFLECT VertexAttribute {
  unsigned slot;
  gl::GLenum type;
  unsigned size;
  unsigned relativeOffset;
  bool normalized;
};

class AG_GL_API VertexArray {
public:
  VertexArray() = default;
  VertexArray(span<VertexAttribute> attribs);
  void initialize(span<VertexAttribute> attribs);

  gl::GLuint object() const { return obj_.get(); }

private:
  GLHandle<VertexArrayDeleter> obj_;
};
}
