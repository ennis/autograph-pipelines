//#include "RectTransform.hpp"
//#include "Scene.hpp"
//#include <algorithm> // std::remove_if

/*int ScenePluginBase::pluginCounter = 0;

Entity Scene::createEntity() {
  uint64_t id;
  if (freeIDs.empty()) {
    entityIDs.push_back(entityIDs.size() + (1ULL << 32));
    id = entityIDs.back();
  } else {
    id = freeIDs.back();
    freeIDs.pop_back();
  }
  return Entity{ this, id };
}

void Scene::deleteEntity(uint64_t eid) {
  // increase generation count
  entityIDs[getEntityIndex(eid)] += 1ULL << 32;
  // add to free list
  freeIDs.push_back(entityIDs[getEntityIndex(eid)]);
}
*/
/*
void collectComponentsHierarchy(Scene &s, ComponentMap<RectTransform> &cm) {
  collectComponentsGeneric(cm, [&](auto it) {
    return !s.isValid(it->first) || !s.isValid(it->second.parent);
  });
}
*/