#include <assimp/Importer.hpp>
#include <assimp/ProgressHandler.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <autograph/engine/Application.h>
#include <autograph/engine/ResourcePool.h>
#include <autograph/engine/Scene.h>

namespace ag {
namespace SceneUtils {

static const char *allowedMeshExtensions[] = {".dae", ".fbx", ".obj", ".3ds"};

//////////////////////////////////////////////////
class AssimpSceneImporter {
public:
  AssimpSceneImporter(EntityList &entities, ResourcePool &resourcePool)
      : entities_{entities}, resourcePool_{resourcePool} {}

  //////////////////////////////////////////////////
  Mesh3D *importAssimpMesh(const aiScene *scene, int index) {
    std::string sceneMeshId =
        std::string{sceneFileId_} + ":mesh" + std::to_string(index);
    return resourcePool_.get_fn<Mesh3D>(sceneMeshId.c_str(), [&](auto) {
      auto mesh = scene->mMeshes[index];
      std::vector<Vertex3D> vertices;
      std::vector<unsigned int> indices;
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
          vertices[i].tangent = vec3{mesh->mTangents[i].x, mesh->mTangents[i].y,
                                     mesh->mTangents[i].z};
      if (mesh->mTextureCoords[0])
        for (unsigned i = 0; i < mesh->mNumVertices; ++i)
          vertices[i].texcoords =
              vec2{mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
      for (unsigned i = 0; i < mesh->mNumFaces; ++i) {
        indices[i * 3 + 0] = mesh->mFaces[i].mIndices[0];
        indices[i * 3 + 1] = mesh->mFaces[i].mIndices[1];
        indices[i * 3 + 2] = mesh->mFaces[i].mIndices[2];
      }
      return ResourceWrapper<Mesh3D>{Mesh3D{vertices, indices}};
    });
  }

  //////////////////////////////////////////////////
  SceneObject *importAssimpNodeRecursive(const aiScene *scene, aiNode *node,
                                         SceneObject *parent) {
    auto entity = entities_.create();
    auto thisNode = entity->addComponent<SceneObject>();
    thisNode->parent = parent;
    aiVector3D scaling;
    aiVector3D position;
    aiQuaternion rotation;
    node->mTransformation.Decompose(scaling, rotation, position);
    thisNode->localTransform.position.x = position.x;
    thisNode->localTransform.position.y = position.y;
    thisNode->localTransform.position.z = position.z;
    thisNode->localTransform.rotation.x = rotation.x;
    thisNode->localTransform.rotation.y = rotation.y;
    thisNode->localTransform.rotation.z = rotation.z;
    thisNode->localTransform.rotation.w = rotation.w;
    thisNode->localTransform.scaling.x = scaling.x;
    thisNode->localTransform.scaling.y = scaling.y;
    thisNode->localTransform.scaling.z = scaling.z;
    if (node->mNumMeshes == 1) {
      thisNode->mesh = importAssimpMesh(scene, node->mMeshes[0]);
      thisNode->localBounds = GetMeshAABB(*thisNode->mesh);
    } else if (node->mNumMeshes > 1) {
      for (unsigned i = 0; i < node->mNumMeshes; ++i) {
        // create sub-objects for the meshes
        auto subEntity = entities_.create();
        auto subObj = subEntity->addComponent<SceneObject>();
        subObj->parent = thisNode;
        subObj->mesh = importAssimpMesh(scene, node->mMeshes[i]);
        subObj->localBounds = GetMeshAABB(*subObj->mesh);
        thisNode->children.push_back(subObj);
      }
    }
    for (unsigned i = 0; i < node->mNumChildren; ++i) {
      thisNode->children.push_back(
          importAssimpNodeRecursive(scene, node->mChildren[i], thisNode));
    }
    AG_DEBUG("[node '{}']", node->mName.C_Str());
    return thisNode;
  }

  //////////////////////////////////////////////////
  SceneObject *loadScene(const char *sceneFileId, SceneObject *parent) {
    Assimp::Importer importer;
    auto actualPath = findResourceFile(sceneFileId, allowedMeshExtensions);
    if (actualPath.empty()) {
      return nullptr;
    }
    const aiScene *scene = importer.ReadFile(
        actualPath.c_str(),
        aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph |
            aiProcess_Triangulate | aiProcess_JoinIdenticalVertices |
            aiProcess_CalcTangentSpace | aiProcess_SortByPType);
    if (!scene) {
      errorMessage("loadScene({}): failed to load scene ({})", sceneFileId, actualPath);
      return nullptr;
    }
    sceneFileId_ = sceneFileId;
    auto ptr = importAssimpNodeRecursive(scene, scene->mRootNode, parent);
    if (parent) {
      parent->children.push_back(ptr);
    }
    sceneFileId_ = nullptr;
    AG_DEBUG("AssimpSceneImporter::loadScene({}): imported {} meshes",
             sceneFileId, scene->mNumMeshes);
    return ptr;
  }

private:
  const char *sceneFileId_{nullptr};
  EntityList &entities_;
  ResourcePool &resourcePool_;
};

//////////////////////////////////////////////////


/*void updateScene() {
  updateWorldTransformsRecursive(mat4{1.0f}, *rootObj_);
  updateObjectBoundsRecursive(*rootObj_);
}
*/
}
}