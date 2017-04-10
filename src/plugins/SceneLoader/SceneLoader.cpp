#include "SceneLoader.Config.h"
#include <assimp/Importer.hpp>
#include <assimp/ProgressHandler.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <autograph/engine/All.h>
#include <autograph/gl/All.h>
#include <autograph/support/FileDialog.h>
#include <autograph/support/ProjectRoot.h>
#include <experimental/filesystem>

using namespace ag;

//////////////////////////////////////////////////
class AssimpSceneImporter {
public:
  AssimpSceneImporter(EntityManager &entities, SceneObjectComponents &scene,
                      RenderableComponents &renderableScene,
                      LightComponents &lights, ResourcePool &resourcePool)
      : entities_{entities}, scene_{scene}, renderableScene_{renderableScene},
        lights_{lights}, resourcePool_{resourcePool} {}

  //////////////////////////////////////////////////
  Mesh3D *importAssimpMesh(const aiScene *aiscene, int index,
                           StdMaterial *outMaterial) {
    std::string sceneMeshId =
        std::string{sceneFileId_} + ":mesh" + std::to_string(index);
    importMaterial(aiscene,
                   aiscene->mMaterials[aiscene->mMeshes[index]->mMaterialIndex],
                   outMaterial);
    return resourcePool_.get_fn<Mesh3D>(sceneMeshId.c_str(), [&](auto) {
      auto mesh = aiscene->mMeshes[index];
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
      // import ref material
      return std::make_unique<ResourceWrapper<Mesh3D>>(
          Mesh3D{vertices, indices});
    });
  }

  void importMaterial(const aiScene *aiscene, const aiMaterial *material,
                      StdMaterial *outMaterial) {
    // get material name
    aiString matName;
    aiString texPath;
    material->Get(AI_MATKEY_NAME, matName);

    /* for (unsigned i = 0; i < material->mNumProperties; ++i) {
    if (material->mProperties[i]->mType == aiPTI_String) {
    AG_DEBUG("[material {} matkey {}]", matName.C_Str(),
    material->mProperties[i]->mKey.C_Str());
    }
    // if (material->mProperties[i]->mSemantic != aiTextureType_NONE)
    }*/

    // load all possible textures
    for (unsigned i = 0; i < AI_TEXTURE_TYPE_MAX; ++i) {
      auto res = material->GetTexture((aiTextureType)i, 0, &texPath);
      if (res == aiReturn_SUCCESS) {
        AG_DEBUG("[material {} texindex{} path {}]", matName.C_Str(), i,
                 texPath.C_Str());
        // Search procedure for textures:
        // First, get an ID from the given path (by taking the filename), and
        // look for it using the default search procedure for resources.
        // If it is not found, resort to absolute file path.
        namespace fs = std::experimental::filesystem;
        fs::path p{texPath.C_Str()};
        auto path = (fs::path{ResourceManager::getParentPath(sceneFileId_)} /
                     p.filename())
                        .generic_string();
        if (ResourceManager::getFilesystemPath(path).empty())
          continue; // path not found in filesystem, continue
        auto tex = resourcePool_.get<gl::Texture>(path.c_str());
        switch (i) {
        case aiTextureType_NONE:
          break;
        case aiTextureType_DIFFUSE:
          outMaterial->albedo = tex;
          break;
        case aiTextureType_SPECULAR:
          outMaterial->metallic = tex;
          break;
        case aiTextureType_AMBIENT:
          break;
        case aiTextureType_EMISSIVE:
          break;
        case aiTextureType_HEIGHT:
          break;
        case aiTextureType_NORMALS:
          outMaterial->normals = tex;
          break;
        case aiTextureType_SHININESS:
          outMaterial->roughness = tex;
          break;
        case aiTextureType_OPACITY:
          // outMaterial->albedo = tex;
          break;
        case aiTextureType_DISPLACEMENT:
          break;
        case aiTextureType_LIGHTMAP:
          break;
        case aiTextureType_REFLECTION:
          break;
        default:
          break;
        }
      }
    }
  }

  //////////////////////////////////////////////////
  SceneObject *importAssimpNodeRecursive(const aiScene *aiscene, aiNode *node,
                                         SceneObject *parent) {
    ID id = entities_.createEntity();
    SceneObject *thisNode = scene_.add(id);
    thisNode->entityID = id;
    thisNode->name = node->mName.C_Str();
    thisNode->parent = parent;
    StdMaterial *mat = renderableScene_.add(id);
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
      thisNode->mesh = importAssimpMesh(aiscene, node->mMeshes[0], mat);
      thisNode->localBounds = GetMeshAABB(*thisNode->mesh);
    } else if (node->mNumMeshes > 1) {
      for (unsigned i = 0; i < node->mNumMeshes; ++i) {
        // create sub-objects for the meshes
        auto subEntity = entities_.createEntity();
        SceneObject *subObj = scene_.add(subEntity);
        StdMaterial *subMat = renderableScene_.add(subEntity);
        subObj->entityID = subEntity;
        subObj->parent = thisNode;
        subObj->mesh = importAssimpMesh(aiscene, node->mMeshes[i], subMat);
        subObj->localBounds = GetMeshAABB(*subObj->mesh);
        thisNode->children.push_back(subObj);
      }
    }
    for (unsigned i = 0; i < node->mNumChildren; ++i) {
      thisNode->children.push_back(
          importAssimpNodeRecursive(aiscene, node->mChildren[i], thisNode));
    }
    AG_DEBUG("[node '{}']", node->mName.C_Str());
    return thisNode;
  }

  Light *importLight(const aiLight *ailight, SceneObject *parent) {
    ID lightEntity = entities_.createEntity();
    SceneObject *lightSceneObj = scene_.add(lightEntity);
    Light *lightObj = lights_.add(lightEntity);
    lightSceneObj->entityID = lightEntity;
    lightSceneObj->name = ailight->mName.C_Str();
    lightSceneObj->hasWorldBounds = false;
    lightSceneObj->localTransform.position =
        vec3{ailight->mPosition.x, ailight->mPosition.y, ailight->mPosition.z};
    if (parent)
      parent->addChild(lightSceneObj);
    AG_DEBUG("[light {}]", ailight->mName.C_Str());
    return lightObj;
  }

  //////////////////////////////////////////////////
  SceneObject *load(const char *sceneFileId, SceneObject *parent) {
    Assimp::Importer importer;
    auto actualPath = ResourceManager::getFilesystemPath(sceneFileId);
    if (actualPath.empty()) {
      return nullptr;
    }
    const aiScene *aiscene = importer.ReadFile(
        actualPath.c_str(),
        aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph |
            aiProcess_Triangulate | aiProcess_JoinIdenticalVertices |
            aiProcess_CalcTangentSpace | aiProcess_SortByPType);
    if (!aiscene) {
      errorMessage("failed to load scene ({}): {}", sceneFileId,
                   importer.GetErrorString());
      return nullptr;
    }
    // import lights
    for (unsigned i = 0; i < aiscene->mNumLights; ++i) {
      importLight(aiscene->mLights[i], parent);
    }

    sceneFileId_ = sceneFileId;
    auto rootSceneObj =
        importAssimpNodeRecursive(aiscene, aiscene->mRootNode, parent);
    if (parent) {
      parent->children.push_back(rootSceneObj);
    }
    sceneFileId_ = nullptr;
    AG_DEBUG("AssimpSceneImporter::loadScene({}): imported {} meshes",
             sceneFileId, aiscene->mNumMeshes);
    return rootSceneObj;
  }

private:
  const char *sceneFileId_{nullptr};
  EntityManager &entities_;
  SceneObjectComponents &scene_;
  RenderableComponents &renderableScene_;
  LightComponents &lights_;
  ResourcePool &resourcePool_;
};

//////////////////////////////////////////////
class AssimpSceneLoader : public ag::SceneLoader {
public:
  virtual bool loadScene(const char *path, Scene &scene, ID &rootObject,
                         ResourcePool &pool,
                         SceneObject *parentObject = nullptr) override {
    auto &entities = scene.getEntityManager();
    auto sceneObjects = scene.getComponentManager<SceneObjectComponents>();
    auto renderableScene = scene.getComponentManager<RenderableComponents>();
    auto lights = scene.getComponentManager<LightComponents>();
    AssimpSceneImporter asi{entities, *sceneObjects, *renderableScene, *lights, pool };
    auto loadedRoot = asi.load(path, parentObject);
    if (!loadedRoot)
      return false;
    rootObject = loadedRoot->entityID;
    return true;
  }

private:
};

//////////////////////////////////////////////
PLUGIN_ENTRY { ag::registerClass<AssimpSceneLoader, ag::Extension>("AssimpSceneLoader"); }
