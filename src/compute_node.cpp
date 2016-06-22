#include "compute_node.hpp"
#include <iostream>
#include <cppformat/format.h>

#include "buffer.hpp"
#include "gl_texture.hpp"
#include "gl_buffer.hpp"
#include "image_impl.hpp"

/*void compute_node::traverse(node_traversal_func fn) {
  for (auto &r : res) {
    if (not_empty(r.access & shader_resource_access::write)) {
		fn(*r.resource);
    }
  }
}
*/