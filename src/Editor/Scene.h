#pragma once
#include <editor/Entity.hpp>
#include <editor/Meta.hpp>
#include <editor/Observable.hpp>
#include <msgpack.hpp>
#include <vector> // std::vector

struct ScenePluginBase {
  static int pluginCounter;
};

template <typename T> struct ScenePlugin : public ScenePluginBase {
  static int index() {
    static int i = ScenePluginBase::pluginCounter++;
    return i;
  }
};

// a container for entities and components
class Scene {
public:
  // arbitrary value
  static constexpr int maxScenePlugins = 40;

  // create a new entity in the scene without any components
  Entity createEntity();
  // delete an entity and its transform children
  void deleteEntity(uint64_t eid);
  // reclaim memory of deleted entities
  void collect();
  // return the number of entities in the scene
  size_t size() const { return entityIDs.size(); }
  // return a view of the entities
  auto &getEntities() { return entityIDs; }

  bool isValid(uint64_t eid) {
	  return (getEntityIndex(eid) < entityIDs.size()) && (getEntityGeneration(eid) == getEntityGeneration(entityIDs[getEntityIndex(eid)]));
  }

  // return all entities in the scene that have a component of type T
  /*template <typename T> auto find(bool include_deleted = false) const {
    using namespace ranges;
    // range-v3 is cool, but incredibly slow to compile
    return ent_ | view::remove_if([include_deleted](auto &&u) {
    return (!u->template getComponent<T>()) ||
    (!include_deleted && u->deleted_);
    }) |
    view::transform([](auto &&e) { return e.get(); });
          static_assert(std::is_base_of<ComponentCounterBase, T>::value,
  "stuff");
    return entity_view{*this, T::family()};
  }*/

  template <typename T> T* addScenePlugin(std::unique_ptr<T> sp) {
    static_assert(std::is_base_of<ScenePlugin<T>, T>());
    return static_cast<T *>(addScenePlugin(T::index(), std::static_pointer_cast<ScenePluginBase>(std::move(sp))));
  }

  ScenePluginBase* addScenePlugin(int index, std::unique_ptr<ScenePluginBase> sp)
  {
	  scenePlugins[index] = std::move(sp);
	  return scenePlugins[index].get();
  }

  template <typename T> T *getScenePlugin() {
    static_assert(std::is_base_of<ScenePlugin<T>, T>(), "T must derive from ScenePlugin<T>");
    auto ptr = static_cast<T *>(getScenePlugin(T::index()));
	if (!ptr) 
		ptr = static_cast<T *>(addScenePlugin(T::index(), std::make_unique<T>()));
	return ptr;
  }

  ScenePluginBase *getScenePlugin(int index) {
    return scenePlugins[index].get();
  }

  template <typename T>
  T* getComponent(uint64_t entity)
  {
        using PluginType = typename T::PluginType;
		using ComponentType = typename T::ComponentType;
		static_assert(std::is_same<T, ComponentType>::value, "T must be a non-derived component type");
		// XXX this is an unsafe cast for AbstractComponents
        return getScenePlugin<PluginType>()->getComponent(entity);
  }

  template <typename T, typename... Args>
  T *addComponent(uint64_t eid, Args &&... args) {
	  using PluginType = typename T::PluginType;
	  return getScenePlugin<PluginType>()->template addComponent<T>(
		  eid, std::forward<Args>(args)...);
  }

  template <typename T>
  void removeComponent(uint64_t eid) {
	  getScenePlugin<PluginType>()->removeComponent(eid);
  }

private:
  std::vector<uint64_t> entityIDs;
  std::vector<uint64_t> freeIDs;
  std::array<std::unique_ptr<ScenePluginBase>, maxScenePlugins> scenePlugins;
};
