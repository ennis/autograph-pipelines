#include "vertex_array.hpp"
#include "device.hpp"
#include <cassert>

namespace ag {
vertex_array::vertex_array(std::initializer_list<vertex_attribute> attribs) {
  initialize(attribs);
}

void vertex_array::initialize(std::initializer_list<vertex_attribute> attribs) {
  constexpr auto max_attribs = 16;
  auto num_attribs = attribs.size();
  GLuint strides[max_attribs] = {0};
  GLuint vertex_array_obj;
  glCreateVertexArrays(1, &vertex_array_obj);
  assert(num_attribs < get_impl_limits().max_vertex_attributes);
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