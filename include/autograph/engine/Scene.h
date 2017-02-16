#pragma once
#include "Mesh.h"
#include <autograph/Transform.h>
#include <autograph/engine/ResourcePool.h>
#include <autograph/support/IDTable.h>
#include <autograph/support/SmallVector.h>
#include <memory>
#include <vector>

struct aiScene;
struct aiMesh;
struct aiNode;

namespace ag {

class ComponentBase {
public:
  virtual ~ComponentBase()
  {}

  virtual int getComponentID() = 0;

protected:
  static int nextComponentID;
};

template <typename Derived> class Component : public ComponentBase {
public:
  static int componentID() {
    static int id = ComponentBase::nextComponentID++;
    return id;
  }
  int getComponentID() override final { return componentID(); }

private:
};

//////////////////////////////////////////////
template <typename T> class EntityListBase {
public:

};

//////////////////////////////////////////////
class Entity {
public:
    friend class EntityList;

	Entity() = default;
	// disable copy
	Entity(const Entity&) = delete;
	Entity& operator=(const Entity&) = delete;
	// move is okay though
	Entity(Entity&&) = default;
	Entity& operator=(Entity&&) = default;

  template <typename T, typename... Args> T *addComponent(Args &&... args) {
    components_.push_back(std::make_unique<T>(std::forward<Args>(args)...));
    // static_assert(T::ComponentID < 8);
    return static_cast<T*>(components_.back().get());
  }

  template <typename T> T *getComponent() {
    for (auto &c : components_) {
      if (c->getComponentID() == T::componentID()) {
        return static_cast<T*>(c.get());
      }
    }
    return nullptr;
  }

  template <typename T> void removeComponent() {
    components_.erase(std::remove_if(components_.begin(), components_.end(),
                                     [](const auto &v) {
                                       return v->getComponentID() ==
                                              T::componentID;
                                     }),
                      components_.end());
  }

  ID getID() const { return id_; }
  const std::string& getName() const { return name_; }
  void setName(const char* name) {  name_ = name; }

private:
  ID id_;
  std::string name_;
  SmallVector<std::unique_ptr<ComponentBase>, 8> components_;
};

//////////////////////////////////////////////
class EntityList
{
public:
  ////////////////////////////////////
  Entity *create(const char* name = nullptr) {
	  if (name == nullptr) 
		  name = "<no name>";
	auto id = ids_.createID();
    auto it = objects_.find(id);
    if (it == objects_.end()) {
        it = objects_.emplace(std::make_pair(id, Entity{})).first;
    }
	it->second.name_ = name;
    it->second.id_ = id;
    return &it->second;
  }

  ////////////////////////////////////
  Entity *get(ID id) {
    auto it = objects_.find(id);
    if (it == objects_.end()) {
      return nullptr;
    } else {
      return &it->second;
    }
  }

  ////////////////////////////////////
  virtual void remove(ID id) { objects_.erase(id); }

  ////////////////////////////////////
  virtual void clear() { objects_.clear(); }

  ////////////////////////////////////
  virtual void clean(const IDTable &idTable) {
    // TODO
  }

  ////////////////////////////////////
  auto &getObjects() { return objects_; }

private:
  std::unordered_map<ID, Entity> objects_;
  IDTable ids_;
};

struct SceneObject : public Component<SceneObject> {
  virtual ~SceneObject();
  void addChild(SceneObject *obj);
  void removeChild(SceneObject *obj);
  void updateBounds();
  void updateWorldTransform(const mat4 &parentTransform = mat4{1.0f});

  ID id;
  SceneObject *parent{nullptr};
  Mesh3D *mesh{nullptr};
  Transform localTransform;
  mat4 worldTransform;
  std::vector<SceneObject *> children;
  AABB worldBounds;
  AABB localBounds;
  bool hasWorldBounds{false};
  // includes children
  AABB getLocalBoundingBox() const;
  AABB getApproximateWorldBoundingBox() const;
};
}
