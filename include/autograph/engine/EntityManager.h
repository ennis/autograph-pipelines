#pragma once
#include <autograph/support/IDTable.h>
#include <unordered_map>

namespace ag {

//////////////////////////////////////////////
class EntityManager {
public:
  ////////////////////////////////////
  ID createEntity() { return ids_.createID(); }
  ID deleteEntity(ID eid) { ids_.deleteID(eid); }

private:
  IDTable ids_;
};

//////////////////////////////////////////////
template <typename T> class ComponentManager {
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