#pragma once
#include <autograph/Config.h>
#include <autograph/support/Span.h>
#include <autograph/gl/GLHandle.h>

namespace ag {
namespace gl {
struct VertexArrayDeleter {
	static constexpr GLenum objectType = GL_VERTEX_ARRAY;
  void operator()(GLuint obj) { glDeleteVertexArrays(1, &obj); }
};

struct AG_REFLECT VertexAttribute {
  unsigned slot;
  GLenum type;
  unsigned size;
  unsigned relativeOffset;
  bool normalized;
};

class AG_API VertexArray {
public:
  VertexArray() = default;
  VertexArray(span<VertexAttribute> attribs);
  void initialize(span<VertexAttribute> attribs);

  GLuint object() const { return obj_.get(); }

private:
  GLHandle<VertexArrayDeleter> obj_;
};
}
}