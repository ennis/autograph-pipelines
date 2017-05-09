#pragma once
#include <autograph/Engine/Exports.h>
#include <autograph/Core/Transform.h>
#include <autograph/Core/AABB.h>
#include <autograph/Engine/EntityManager.h>
#include <autograph/Engine/Mesh.h>
#include <unordered_map>
#include <string>
#include <vector>

namespace ag
{
struct AG_ENGINE_API SceneObject {
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

class AG_ENGINE_API SceneObjectComponents : public ComponentManager<SceneObject> {
public:
	// set a parent-child relationship between two entities
	void parent(ID parent, ID child);
	// calculate world transforms
	void update(const mat4& baseTransform = mat4{ 1.0f });
	void showGUI(ID id) override;
};

}