#pragma once
#include <autograph/Types.h>
#include <autograph/support/Span.h>
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
	Mesh() = default;
	Mesh(span<Vertex3D> vertices, span<unsigned int> indices);

  static Mesh loadFromFile(const char* path);

    const AABB& getAABB() const {
        return aabb_;
    }

	auto& getVertices() { return vertices_; }
	auto& getIndices() { return indices_; }

	int getVerticesCount() const { return numVertices_; }
	int getIndicesCount() const { return numIndices_; }

	auto getVertexBuffer() { 
		return gl::BufferSlice{ vbo_.object(), 0, vbo_.size() };
	}

	auto getIndexBuffer() {
		return gl::BufferSlice{ ibo_.object(), 0, ibo_.size() };
	}

private:
  std::vector<Vertex3D> vertices_;
  std::vector<unsigned int> indices_;
  int numVertices_{ 0 };
  int numIndices_{ 0 };
  gl::Buffer vbo_;
  gl::Buffer ibo_;
  AABB aabb_;
};

}
