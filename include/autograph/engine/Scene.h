#pragma once
#include <autograph/Transform.h>
#include <autograph/AABB.h>
#include <autograph/engine/EntityManager.h>
#include <autograph/engine/Mesh.h>
#include <unordered_map>
#include <string>
#include <vector>

namespace ag
{
struct SceneObject {
  //========================
  // DATA
  ID entityID;	// owning entity ID
  std::string name;
  SceneObject *parent = nullptr;
  Mesh3D *mesh = nullptr;
  Transform localTransform;
  mat4 worldTransform;
  std::vector<SceneObject *> children;
  AABB worldBounds;
  AABB localBounds;
  bool hasWorldBounds = false;

  //========================
  // METHODS

  ~SceneObject();
  // add a child SceneObject
  void addChild(SceneObject *obj);
  // removes a child
  void removeChild(SceneObject *obj);
  // recalculate the world-space bounding boxes of this element and its children
  void calculateWorldBounds();
  // recalculate the model-to-world transformation (worldTransform) of this
  // element and its children
  void calculateWorldTransform(const mat4 &parentTransform = mat4{1.0f});
  // includes children
  AABB getLocalBounds() const;
  AABB getApproximateWorldBounds() const;
};

class Scene : public ComponentManager<SceneObject> {
public:
  // set a parent-child relationship between two entities
  void parent(ID parent, ID child);
};
}