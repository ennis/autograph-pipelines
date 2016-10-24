#pragma once
#include <autograph/Types.h>
#include <vector>

namespace ag {
struct Mesh {
  std::vector<Vertex3D> vertices;
  std::vector<unsigned int> indices;
  gl_buffer vbo;
  gl_buffer ibo;
  static Mesh load(const std::experimental::filesystem::path &path, AABB &aabb);
};
}