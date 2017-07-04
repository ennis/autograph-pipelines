#pragma once
#include <autograph/Core/AABB.h>
#include <autograph/Core/Types.h>
#include <autograph/Gfx/Bind.h>
#include <autograph/Gfx/Buffer.h>
#include <autograph/Core/Support/Span.h>
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

/// 
/// Mesh
/// TODO Documentation: a class that holds an array of vertices and an array of indices
/// both into main memory and into a GPU buffer. Used for rendering meshes, can be bound to a pipeline
/// Describe how an instance of Mesh binds to a pipeline: sets vertexBuffer(0), indexBuffer, drawCommand issued
template <typename T> class Mesh 
{
public:
  Mesh() = default;

  /// TODO Documentation
  /// Constructor from a vectors of vertices and indices 
  Mesh(std::vector<T> vertices, std::vector<unsigned int> indices) {
    vbo_ = Buffer{vertices.size() * sizeof(T), Buffer::Usage::Default,
                      vertices.data()};
    ibo_ = Buffer{indices.size() * sizeof(unsigned int),
                      Buffer::Usage::Default, indices.data()};
    vertices_ = std::move(vertices);
    indices_ = std::move(indices);
  }

  /// TODO Documentation
  /// Constructor from a slice of vertices and indices
  Mesh(span<const T> vertices, span<unsigned int> indices) {
    vbo_ = Buffer{vertices.size() * sizeof(T), Buffer::Usage::Default,
                      vertices.data()};
    ibo_ = Buffer{indices.size() * sizeof(unsigned int),
                      Buffer::Usage::Default, indices.data()};
    vertices_.assign(vertices.begin(), vertices.end());
    indices_.assign(indices.begin(), indices.end());
  }

  /// TODO documentation
  /// binder (draw command)
  void operator()(StateGroup &sg) {
    bind::vertexBuffer(0, vbo_.asSlice(), sizeof(T))(sg);
    bind::indexBuffer(ibo_.asSlice(), gl::UNSIGNED_INT)(sg);
    bindStateGroup(sg);
    auto indexStride = 4;
    gl::DrawElementsBaseVertex(gl::TRIANGLES, indices_.size(), gl::UNSIGNED_INT,
                           ((const char *)((uintptr_t)0 * indexStride)), 0);
  }

  auto &vertices() const { return span<const T>(vertices_); }
  auto &indices() const { return span<const unsigned int>(indices_); }

  /// equivalent to vertices.size()
  int vertexCount() const { return (int)vertices_.size(); }
  /// equivalent to indices.size()
  int indexCount() const { return (int)indices_.size(); }

  /// Returns the slice of the GPU buffer containing vertex data
  auto vertexBuffer() const { return Buffer::Slice{vbo_.object(), 0, vbo_.size()}; }
  /// Returns the slice of the GPU buffer containing index data
  auto indexBuffer() const { return Buffer::Slice{ibo_.object(), 0, ibo_.size()}; }

private:
  std::vector<T> vertices_;
  std::vector<unsigned int> indices_;
  Buffer vbo_;
  Buffer ibo_;
};

/// TODO Documentation
/// Returns the axis-aligned bounding box (AABB) of a set of vertices
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

/// TODO Documentation
/// Returns the axis-aligned bounding box (AABB) of a mesh
template <typename VertexType> AABB GetMeshAABB(Mesh<VertexType> &mesh) {
  return GetMeshAABB(mesh.vertices());
}

/// Specialization of Mesh for vertices in 3D (position, normals, tangents, texcoords)
using Mesh3D = Mesh<Vertex3D>;
/// Specialization of Mesh for vertices in 2D + texture coordinates
using Mesh2DTex = Mesh<Vertex2DTex>;
/// Specialization of Mesh for vertices in 2D
using Mesh2D = Mesh<Vertex2D>;

} // namespace ag
