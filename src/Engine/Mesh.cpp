#include <autograph/Engine/Mesh.h>
#include <autograph/Core/Support/Debug.h>
#include <stdexcept>

namespace ag {

	/*
Mesh Mesh::loadFromFile(const char *path) {
  Assimp::Importer importer;

  AG_DEBUG("Mesh::loadFromFile {}", path);

  const aiScene *scene = importer.ReadFile(
      path, aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph |
                           aiProcess_Triangulate |
                           aiProcess_JoinIdenticalVertices |
                           aiProcess_CalcTangentSpace | aiProcess_SortByPType);

  if (!scene) {
      // Could not load scene, return an empty mesh
      errorMessage("Mesh::loadFromFile: could not load scene {}", path);
    return Mesh{};
  }


  // load the first mesh in the file
  std::vector<Vertex3D> vertices;
  std::vector<unsigned int> indices;
  if (scene->mNumMeshes > 0) {
    auto mesh = scene->mMeshes[0];
    vertices.resize(mesh->mNumVertices);
    indices.resize(mesh->mNumFaces * 3);
    for (unsigned i = 0; i < mesh->mNumVertices; ++i) {
      auto v = mesh->mVertices[i];
      vertices[i].position = vec3{v.x, v.y, v.z};
    }
    if (mesh->mNormals)
      for (unsigned i = 0; i < mesh->mNumVertices; ++i)
        vertices[i].normal = vec3{mesh->mNormals[i].x, mesh->mNormals[i].y,
                                       mesh->mNormals[i].z};
    if (mesh->mTangents)
      for (unsigned i = 0; i < mesh->mNumVertices; ++i)
        vertices[i].tangent = vec3{
            mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z};
    if (mesh->mTextureCoords[0])
      for (unsigned i = 0; i < mesh->mNumVertices; ++i)
        vertices[i].texcoords = vec2{mesh->mTextureCoords[0][i].x,
                                          mesh->mTextureCoords[0][i].y};
    for (unsigned i = 0; i < mesh->mNumFaces; ++i) {
      indices[i * 3 + 0] = mesh->mFaces[i].mIndices[0];
      indices[i * 3 + 1] = mesh->mFaces[i].mIndices[1];
      indices[i * 3 + 2] = mesh->mFaces[i].mIndices[2];
    }
  }

  AG_DEBUG("Mesh::loadFromFile {} vertices:{} indices:{}", path, vertices.size(), indices.size());
  return Mesh{ vertices, indices };
}*/

}
