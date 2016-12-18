#include <autograph/gl/Device.h>
#include <autograph/gl/VertexArray.h>
#include <cassert>

namespace ag {
namespace gl {
VertexArray::VertexArray(ag::span<VertexAttribute> attribs) {
  initialize(attribs);
}

void VertexArray::initialize(ag::span<VertexAttribute> attribs) {
  constexpr auto max_attribs = 16;
  auto num_attribs = attribs.size();
  GLuint strides[max_attribs] = {0};
  GLuint vertex_array_obj;
  glCreateVertexArrays(1, &vertex_array_obj);
  assert(num_attribs < getGLImplementationLimits().max_vertex_attributes);
  int attribindex = 0;
  for (auto &&a : attribs) {
    assert(a.slot < max_attribs);
    glEnableVertexArrayAttrib(vertex_array_obj, attribindex);
    glVertexArrayAttribFormat(vertex_array_obj, attribindex, a.size, a.type,
                              a.normalized, strides[a.slot]);
    glVertexArrayAttribBinding(vertex_array_obj, attribindex, a.slot);
    strides[a.slot] += a.stride;
    ++attribindex;
  }
  obj_ = vertex_array_obj;
}
}
}