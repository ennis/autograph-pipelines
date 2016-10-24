#pragma once

/*
template <typename T>
using ComponentMap = boost::container::flat_map<uint64_t, T>;

template <typename T> T *findComponent(ComponentMap<T> &map, uint64_t key) {
  auto it = map.find(key);
  if (it == map.end()) {
    return nullptr;
  }
  return &it->second;
}

template <typename T, typename F>
void collectComponentsGeneric(ComponentMap<T> &cm, F condition) {
  for (auto it = cm.begin(); it != cm.end();) {
    if (condition(it)) {
      it = cm.erase(it);
    } else {
      it++;
    }
  }
}

template <typename T> void collectComponents(Scene &s, ComponentMap<T> &cm) {
  collectComponentsGeneric(cm, [&](auto it) { return !s.isValid(it->first); });
}

// A convenience scene plugin that manages a component type in a scene
template <typename T>
struct ComponentScenePlugin : public ScenePlugin<ComponentScenePlugin<T>> {
  template <typename T, typename... Args> T *addComponent(uint64_t eid, Args &&... args) {
	auto& r = (data_[eid] = T{std::forward<Args>(args)...});
	return &r;
  }

  void removeComponent(uint64_t eid) { data_.erase(eid); }

  T *getComponent(uint64_t eid) {
    auto it = data_.find(eid);
    if (it == data_.end()) {
      return nullptr;
    }
    return &it->second;
  }

  // garbage collection
  void collect(Scene &s) { collectComponents(s, data_); }

  ComponentMap<T> data_;
};

// A convenience scene plugin that manages an abstract component type in a scene
// (adds one pointer indirection)
template <typename T>
struct AbstractComponentScenePlugin
    : public ScenePlugin<AbstractComponentScenePlugin<T>> {
  template <typename U, typename... Args>
  U *addComponent(uint64_t eid, Args &&... args) {
	  auto& r = (data_[eid] = std::make_unique<U>(std::forward<Args>(args)...));
	return static_cast<U*>(r.get());
  }

  void removeComponent(uint64_t eid) { data_.erase(eid); }

  T *getComponent(uint64_t eid) {
    auto it = data_.find(eid);
    if (it == data_.end()) {
      return nullptr;
    }
    return it->second.get();
  }

  // garbage collection
  void collect(Scene &s) { collectComponents(s, data_); }

  ComponentMap<std::unique_ptr<T>> data_;
};

template <typename Derived> struct Component {
  using PluginType = ComponentScenePlugin<Derived>;
  using ComponentType = Derived;

  /*template <typename... Args>
  static Derived *add(Scene &s, uint64_t eid, Args &&... args) {
    return s.getScenePlugin<PluginType>()->addComponent(
        eid, std::forward<Args>(args)...);
  }

  static PluginType *attach(Scene &s) {
    auto ptr = std::make_unique<PluginType>();
    auto pptr = ptr.get();
    s.addScenePlugin(std::move(ptr));
    return pptr;
  }

  static Derived *get(Scene &s, uint64_t eid) {
    return s.getScenePlugin<PluginType>()->getComponent(eid);
  }

  static void remove(Scene &s, uint64_t eid) {
    s.getScenePlugin<PluginType>()->removeComponent(eid);
  }

  static void collectGarbage(Scene &s) {
    s.getScenePlugin<PluginType>()->collectGarbage(s);
  }
};

template <typename Derived> struct AbstractComponent {
	using PluginType = AbstractComponentScenePlugin<Derived>;
	using ComponentType = Derived;
};
*/