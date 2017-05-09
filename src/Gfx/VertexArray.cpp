#include <autograph/Gfx/GfxContext.h>
#include <autograph/Gfx/VertexArray.h>
#include <cassert>

namespace ag {

VertexArray::VertexArray(ag::span<VertexAttribute> attribs) {
  initialize(attribs);
}

void VertexArray::initialize(ag::span<VertexAttribute> attribs) {
  constexpr auto max_attribs = 16;
  auto num_attribs = attribs.size();
 // GLuint strides[max_attribs] = {0};
  gl::GLuint vertex_array_obj;
  gl::CreateVertexArrays(1, &vertex_array_obj);
  assert(num_attribs < getGfxContext().getGLImplementationLimits().max_vertex_attributes);
  int attribindex = 0;
  for (auto &&a : attribs) {
    assert(a.slot < max_attribs);
    gl::EnableVertexArrayAttrib(vertex_array_obj, attribindex);
    gl::VertexArrayAttribFormat(vertex_array_obj, attribindex, a.size, a.type,
                              a.normalized, a.relativeOffset);
    gl::VertexArrayAttribBinding(vertex_array_obj, attribindex, a.slot);
    //strides[a.slot] += a.stride;
    ++attribindex;
  }
  obj_ = vertex_array_obj;
}

}
