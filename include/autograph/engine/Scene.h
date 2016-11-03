#pragma once

#include <autograph/Transform.h>
#include <autograph/Types.h>
#include <autograph/support/ilist.h>
#include <autograph/support/ilist_node.h>

namespace ag {
struct Visual : public ilist_node<Visual> {
  uint64_t id;
  // parent visual
  Visual *parent;
  // siblings
  Visual *prev;
  Visual *next;
  // local transform (relative to parent elements)
  Transform transform;
  // model to world transform (flattened)
  mat4 cachedTransform;
};

// A container for a hierarchy of visual elements
class Scene {
public:
  // create or add a visual
  Visual &addVisual(uint64_t id);
  // get visual or nullptr
  Visual *getVisual(uint64_t id);

private:
  // Container for visual nodes
  // ID->Visual
  IDMap<Visual> visuals;
};
}