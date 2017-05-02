#include <autograph/Engine/SceneObject.h>
#include <autograph/Engine/imgui.h>
#include <algorithm>  // remove

namespace ag {

SceneObject::~SceneObject() {
  if (parent) {
    parent->removeChild(this);
  }
  for (auto c : children) {
    c->parent = nullptr;
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

AABB SceneObject::getLocalBounds() const {
  if (mesh)
    return localBounds;
  else
    return AABB{};
}

AABB SceneObject::getApproximateWorldBounds() const {
  if (hasWorldBounds) {
    return worldBounds;
  }
  return AABB{};
}

void SceneObject::calculateWorldBounds() {
  if (mesh) {
    worldBounds = getLocalBounds().transform(worldTransform);
    hasWorldBounds = true;
  } else {
    hasWorldBounds = false;
  }
  for (auto c : children) {
    c->calculateWorldBounds();
    if (c->hasWorldBounds) {
      if (!hasWorldBounds)
        worldBounds = c->worldBounds;
      else
        worldBounds.unionWith(c->worldBounds);
      hasWorldBounds = true;
    }
  }
}

void SceneObject::calculateWorldTransform(const mat4 &parentTransform) {
  mat4 current = parentTransform;
  current *= localTransform.getMatrix();
  worldTransform = current;
  for (auto c : children) {
    c->calculateWorldTransform(current);
  }
}

// set a parent-child relationship between two entities
void SceneObjectComponents::parent(ID parent, ID child) {
  auto pparent = get(parent);
  auto pchild = get(child);
  pparent->addChild(pchild);
}

// calculate world transforms
void SceneObjectComponents::update(const mat4& baseTransform)
{
	for (auto& obj : getObjects()) {
		if (!obj.second.parent) {
			obj.second.calculateWorldTransform(baseTransform);
			obj.second.calculateWorldBounds();
		}
	}
}

void SceneObjectComponents::showGUI(ID id)
{
	if (auto p = get(id)) {
		if (ImGui::CollapsingHeader("SceneObject")) {
			ImGui::TextDisabled("TODO...");
		}
	}
}


}
