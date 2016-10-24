#pragma once
#include <autograph/Config.h>
#include <autograph/gl/GLHandle.h>

namespace ag {
namespace gl {
struct VertexArrayDeleter {
  void operator()(GLuint obj) { glDeleteVertexArrays(1, &obj); }
};

struct VertexAttribute {
  unsigned slot;
  GLenum type;
  unsigned size;
  unsigned stride;
  bool normalized;
};

class AG_API VertexArray {
public:
  VertexArray() = default;
  VertexArray(std::initializer_list<VertexAttribute> attribs);
  void initialize(std::initializer_list<VertexAttribute> attribs);

  GLuint object() const { return obj_.get(); }

private:
  GLHandle<VertexArrayDeleter> obj_;
};
}
}