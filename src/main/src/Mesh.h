#pragma once
#include <autograph/Types.h>
#include <autograph/AABB.h>
#include <autograph/gl/Buffer.h>
#include <vector>

namespace ag 
{

struct Vertex3D {
  vec3 position;
  vec3 normal;
  vec3 tangent;
  vec2 texcoords;
};

class Mesh 
{
public:
  static Mesh loadFromFile(const char* path);
    const AABB& getAABB() const {
        return aabb_;
    }

private:
  std::vector<Vertex3D> vertices_;
  std::vector<unsigned int> indices_;
  gl::Buffer vbo_;
  gl::Buffer ibo_;
  AABB aabb_;
};

}
