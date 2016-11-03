#pragma once
#include <autograph/Types.h>
#include <autograph/gl/Buffer.h>
#include <vector>

namespace ag {

class Mesh {
public:
  static Mesh load(const char *path, AABB &aabb);
  
private:
  std::vector<Vertex3D> vertices;
  std::vector<unsigned int> indices;
  gl::Buffer vertexBuffer;
  gl::Buffer indexBuffer;
};

}