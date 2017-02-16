
#include <autograph/engine/Application.h>
#include <autograph/engine/Scene.h>
#include <autograph/support/Debug.h>

namespace ag {

int ComponentBase::nextComponentID = 0;

SceneObject::~SceneObject() {
  if (parent) {
    parent->removeChild(this);
  }
}

void SceneObject::addChild(SceneObject *obj) {
  obj->parent = this;
  children.push_back(obj);
}

void SceneObject::removeChild(SceneObject *obj) {
  children.erase(std::remove(children.begin(), children.end(), obj),
                 children.end());
}

AABB SceneObject::getLocalBoundingBox() const {
  if (mesh)
    return localBounds;
  else
    return AABB{};
}

AABB SceneObject::getApproximateWorldBoundingBox() const {
  if (hasWorldBounds) {
    return worldBounds;
  }
  return AABB{};
}

void SceneObject::updateBounds() {
  if (mesh) {
    worldBounds = getLocalBoundingBox().transform(worldTransform);
    hasWorldBounds = true;
  } else {
    hasWorldBounds = false;
  }
  for (auto c : children) {
    c->updateBounds();
    if (c->hasWorldBounds) {
      if (!hasWorldBounds)
        worldBounds = c->worldBounds;
      else
        worldBounds.unionWith(c->worldBounds);
      hasWorldBounds = true;
    }
  }
}

void SceneObject::updateWorldTransform(const mat4 &parentTransform) {
  mat4 current = parentTransform;
  current *= localTransform.getMatrix();
  worldTransform = current;
  for (auto c : children) {
    c->updateWorldTransform(current);
  }
}

}
