#include <autograph/engine/Scene.h>
#include <assimp/Importer.hpp>
#include <assimp/ProgressHandler.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <autograph/engine/Application.h>
#include <autograph/support/Debug.h>

namespace ag {
class ProgressHandler : public Assimp::ProgressHandler {
public:
  virtual bool Update(float percentage) override {
    AG_DEBUG("ReadFile: {} %", percentage);
  }
};

Scene::Scene() { rootObj_ = &addObject(); }

void Scene::clear() { sceneObjects_.clear(); }

auto Scene::addObject() -> SceneObject & {
  auto obj = std::make_unique<SceneObject>();
  auto ptr = obj.get();
  sceneObjects_.push_back(std::move(obj));
  return *ptr;
}

SceneObject &Scene::addMesh(Mesh &mesh) {
  auto &obj = addObject();
  obj.mesh = &mesh;
  obj.id = 0;
  return obj;
}

auto Scene::makeOwnedMesh(std::unique_ptr<Mesh> pMesh) -> Mesh * {
  auto ptr = pMesh.get();
  ownedMeshes_.push_back(std::move(pMesh));
  return ptr;
}

static const char *allowedMeshExtensions[] = {".dae", ".fbx", ".obj", ".3ds"};

SceneObject &Scene::loadModel(const char *id) {
  Assimp::Importer importer;

  auto actualPath = FindResourceFile(id, allowedMeshExtensions);

  const aiScene *scene = importer.ReadFile(
      actualPath.c_str(),
      aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph |
          aiProcess_Triangulate | aiProcess_JoinIdenticalVertices |
          aiProcess_CalcTangentSpace | aiProcess_SortByPType);

  if (!scene) {
    errorMessage("Scene::loadModel({}): failed to load scene ({})", id,
                 actualPath);
    errorMessage("assimp: {}", importer.GetErrorString());
    throw std::runtime_error("failed to load scene");
  }

  std::vector<Mesh *> loadedMeshes{scene->mNumMeshes, nullptr};
  auto ptr = importAssimpNodeRecursive(scene, scene->mRootNode, rootObj_,
                                       loadedMeshes.data());
  rootObj_->children.push_back(ptr);
  AG_DEBUG("Scene::loadModel({}): imported {} meshes", id, scene->mNumMeshes);
  update();
  return *ptr;
}

auto Scene::importAssimpMesh(const aiScene *scene, int index,
                             Mesh **loadedMeshes) -> Mesh * {
  if (auto ptr = loadedMeshes[index])
    return ptr;
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
      vertices[i].normal =
          vec3{mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};
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
  return loadedMeshes[index] =
             makeOwnedMesh(std::make_unique<Mesh>(vertices, indices));
}

auto Scene::importAssimpNodeRecursive(const aiScene *scene, aiNode *node,
                                      SceneObject *parent, Mesh **loadedMeshes)
    -> SceneObject * {
  auto &thisNode = addObject();
  thisNode.parent = parent;
  aiVector3D scaling;
  aiVector3D position;
  aiQuaternion rotation;
  node->mTransformation.Decompose(scaling, rotation, position);
  thisNode.localTransform.position.x = position.x;
  thisNode.localTransform.position.y = position.y;
  thisNode.localTransform.position.z = position.z;
  thisNode.localTransform.rotation.x = rotation.x;
  thisNode.localTransform.rotation.y = rotation.y;
  thisNode.localTransform.rotation.z = rotation.z;
  thisNode.localTransform.rotation.w = rotation.w;
  thisNode.localTransform.scaling.x = scaling.x;
  thisNode.localTransform.scaling.y = scaling.y;
  thisNode.localTransform.scaling.z = scaling.z;
  if (node->mNumMeshes == 1) {
    thisNode.mesh = importAssimpMesh(scene, node->mMeshes[0], loadedMeshes);
  } else if (node->mNumMeshes > 1) {
    for (unsigned i = 0; i < node->mNumMeshes; ++i) {
      // create sub-objects for the meshes
      auto &subObj = addObject();
      subObj.parent = &thisNode;
      subObj.mesh = importAssimpMesh(scene, node->mMeshes[i], loadedMeshes);
      thisNode.children.push_back(&subObj);
    }
  }
  for (unsigned i = 0; i < node->mNumChildren; ++i) {
    thisNode.children.push_back(importAssimpNodeRecursive(
        scene, node->mChildren[i], &thisNode, loadedMeshes));
  }
  // AG_DEBUG("[node '{}']", node->mName.C_Str());
  return &thisNode;
}

void Scene::update() {
  updateWorldTransformsRecursive(mat4{1.0f}, *rootObj_);
  updateObjectBoundsRecursive(*rootObj_);
}

void Scene::updateObjectBoundsRecursive(SceneObject &obj) {
  if (obj.mesh) {
    obj.worldBounds = obj.getLocalBoundingBox().transform(obj.worldTransform);
    obj.hasWorldBounds = true;
  } else {
    obj.hasWorldBounds = false;
  }
  for (auto c : obj.children) {
    updateObjectBoundsRecursive(*c);
    if (c->hasWorldBounds) {
      if (!obj.hasWorldBounds)
        obj.worldBounds = c->worldBounds;
      else
        obj.worldBounds.unionWith(c->worldBounds);
      obj.hasWorldBounds = true;
    }
  }
}

void Scene::updateWorldTransformsRecursive(mat4 current, SceneObject &obj) {
  current *= obj.localTransform.getMatrix();
  obj.worldTransform = current;
  for (auto &&c : obj.children) {
    updateWorldTransformsRecursive(current, *c);
  }
}

AABB SceneObject::getLocalBoundingBox() const {
  if (mesh)
    return mesh->getAABB().transform(localTransform.getMatrix());
  else
    return AABB{};
}

AABB SceneObject::getApproximateWorldBoundingBox() const {
  if (hasWorldBounds) {
    return worldBounds;
  }
  return AABB{};
}
}