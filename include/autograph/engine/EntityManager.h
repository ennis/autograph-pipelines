#pragma once
#include <autograph/engine/Config.h>
#include <autograph/support/IDTable.h>
#include <unordered_map>
#include <set>
#include <typeindex>

namespace ag {

//////////////////////////////////////////////
class AG_ENGINE_API EntityManager {
public:
  ////////////////////////////////////
  ID createEntity() { return ids_.createID(); }
  void deleteEntity(ID eid) { ids_.deleteID(eid); }

private:
  IDTable ids_;
};

class AG_ENGINE_API ComponentManagerBase
{
public:
	virtual void serialize(ID id) {}
	virtual void deserialize(ID id) {}
	virtual void showGUI(ID id) {}
private:
};

//////////////////////////////////////////////
template <typename T> class ComponentManager : public ComponentManagerBase {
public:
  ////////////////////////////////////
  virtual T *add(ID id) { return add(id, T{}); }

  ////////////////////////////////////
  virtual T *add(ID id, const T &obj) {
    auto it = objects_.find(id);
    if (it == objects_.end()) {
      it = objects_.emplace(std::make_pair(id, obj)).first;
    } else {
      it->second = obj;
    }
    return &it->second;
  }

  ////////////////////////////////////
  virtual T *get(ID id) {
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
  virtual void clean(const EntityManager &entityManager) {
    // TODO
  }

  ////////////////////////////////////
  auto &getObjects() { return objects_; }

private:
  std::unordered_map<ID, T> objects_;
};

class AG_ENGINE_API Scene
{
public:
	Scene(EntityManager& entityManager);
	void registerComponentManager(ComponentManagerBase& componentManager);
	void unregisterComponentManager(ComponentManagerBase& componentManager);
	ComponentManagerBase* getComponentManager(std::type_index ti);
	template <typename T> T* getComponentManager() {
		return static_cast<T*>(getComponentManager(typeid(T)));
	}
	void showGUI(ID id);
	EntityManager& getEntityManager() { return entityManager_; }
	const EntityManager& getEntityManager() const { return entityManager_; }
	// serialize / deserialize

private:
	EntityManager& entityManager_;
	std::set<ComponentManagerBase*> componentManagers_;
};


//////////////////////////////////////////////
/*template <typename T> class AbstractComponentManager {
public:
	////////////////////////////////////
	virtual T *add(ID id, std::unique_ptr<T> obj) {
		auto idx = IDIndex(id);
		objects_[idx] = std::move(obj);
		auto it = objects_.find(idx);
		if (it == objects_.end()) {
			it = objects_.emplace(std::make_pair(idx, obj)).first;
		}
		else {
			it->second = obj;
		}
		return &it->second;
	}

	////////////////////////////////////
	virtual T *get(ID id) {
		auto it = objects_.find(id);
		if (it == objects_.end()) {
			return nullptr;
		}
		else {
			return &it->second;
		}
	}

	////////////////////////////////////
	virtual void remove(ID id) { objects_.erase(id); }

	////////////////////////////////////
	virtual void clear() { objects_.clear(); }

	////////////////////////////////////
	virtual void clean(const EntityManager &entityManager) {
		// TODO
	}

	////////////////////////////////////
	auto &getObjects() { return objects_; }

private:
	std::unordered_map<ID, std::unique_ptr<T>> objects_;
};*/

}
