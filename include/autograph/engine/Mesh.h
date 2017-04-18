#pragma once
#include <autograph/AABB.h>
#include <autograph/Types.h>
#include <autograph/gl/Bind.h>
#include <autograph/gl/Buffer.h>
#include <autograph/support/Span.h>
#include <vector>

namespace ag {

struct Vertex2D {
  vec2 position;
};

struct Vertex2DTex {
  vec2 position;
  vec2 tex;
};

struct Vertex3D {
  vec3 position;
  vec3 normal;
  vec3 tangent;
  vec2 texcoords;
};

template <typename T> struct VertexTraits;

template <> struct VertexTraits<Vertex2D> {
  static constexpr int dimensions = 2;
  static vec2 position(const Vertex2D &v) { return v.position; }
};

template <> struct VertexTraits<Vertex2DTex> {
  static constexpr int dimensions = 2;
  static vec2 position(const Vertex2DTex &v) { return v.position; }
};

template <> struct VertexTraits<Vertex3D> {
  static constexpr int dimensions = 3;
  static vec3 position(const Vertex3D &v) { return v.position; }
};

template <typename T> class Mesh {
public:
  Mesh() = default;
  Mesh(std::vector<T> vertices, std::vector<unsigned int> indices) {
    vbo_ = Buffer{vertices.size() * sizeof(T), BufferUsage::Default,
                      vertices.data()};
    ibo_ = Buffer{indices.size() * sizeof(unsigned int),
                      BufferUsage::Default, indices.data()};
    numVertices_ = (int)vertices.size();
    numIndices_ = (int)indices.size();
    vertices_ = std::move(vertices);
    indices_ = std::move(indices);
  }

  Mesh(span<const T> vertices, span<unsigned int> indices) {
    vbo_ = Buffer{vertices.size() * sizeof(T), BufferUsage::Default,
                      vertices.data()};
    ibo_ = Buffer{indices.size() * sizeof(unsigned int),
                      BufferUsage::Default, indices.data()};
    numVertices_ = vertices.size();
    numIndices_ = indices.size();
    vertices_.assign(vertices.begin(), vertices.end());
    indices_.assign(indices.begin(), indices.end());
  }

  // static Mesh loadFromFile(const char* path);

  /*const AABB& getAABB() const {
      return aabb_;
  }*/

  // binder (draw command)
  void operator()(StateGroup &sg) {
    bind::vertexBuffer(0, vbo_.asSlice(), sizeof(T))(sg);
    bind::indexBuffer(ibo_.asSlice(), gl::UNSIGNED_INT)(sg);
    bindStateGroup(sg);
    auto indexStride = 4;
    gl::DrawElementsBaseVertex(gl::TRIANGLES, numIndices_, gl::UNSIGNED_INT,
                           ((const char *)((uintptr_t)0 * indexStride)), 0);
  }

  auto &getVertices() { return vertices_; }
  auto &getIndices() { return indices_; }

  int getVerticesCount() const { return numVertices_; }
  int getIndicesCount() const { return numIndices_; }

  auto getVertexBuffer() { return BufferSlice{vbo_.object(), 0, vbo_.size()}; }

  auto getIndexBuffer() { return BufferSlice{ibo_.object(), 0, ibo_.size()}; }

private:
  std::vector<T> vertices_;
  std::vector<unsigned int> indices_;
  int numVertices_{0};
  int numIndices_{0};
  Buffer vbo_;
  Buffer ibo_;
};

template <typename VertexType>
AABB GetMeshAABB(span<const VertexType> vertices) {
  static_assert(VertexTraits<VertexType>::dimensions == 3,
                "Vertex dimension must be 3");
  AABB aabb;
  // init AABB to reasonably unreasonable values
  aabb.xmin = 1000000.0f;
  aabb.xmax = -1000000.0f;
  aabb.ymin = 1000000.0f;
  aabb.ymax = -1000000.0f;
  aabb.zmin = 1000000.0f;
  aabb.zmax = -1000000.0f;
  for (unsigned i = 0; i < vertices.size(); ++i) {
    auto v = vertices[i];
    auto pos = VertexTraits<VertexType>::position(v);
    if (aabb.xmin > pos.x)
      aabb.xmin = pos.x;
    if (aabb.xmax < pos.x)
      aabb.xmax = pos.x;
    if (aabb.ymin > pos.y)
      aabb.ymin = pos.y;
    if (aabb.ymax < pos.y)
      aabb.ymax = pos.y;
    if (aabb.zmin > pos.z)
      aabb.zmin = pos.z;
    if (aabb.zmax < pos.z)
      aabb.zmax = pos.z;
  }
  return aabb;
}

template <typename VertexType> AABB GetMeshAABB(Mesh<VertexType> &mesh) {
  return GetMeshAABB(span<const VertexType>{mesh.getVertices()});
}

using Mesh3D = Mesh<Vertex3D>;
using Mesh2DTex = Mesh<Vertex2DTex>;
using Mesh2D = Mesh<Vertex2D>;
} // namespace ag
