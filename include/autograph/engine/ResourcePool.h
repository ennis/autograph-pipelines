#pragma once
#include <autograph/Config.h>
#include <chrono>
#include <memory>
#include <unordered_map>

namespace ag {
struct Resource {
  virtual ~Resource() {}
  virtual void *getPtr() = 0;

  std::string id;
  std::chrono::system_clock::time_point lastReload;
};

template <typename T> struct ResourceTraits;

// wrap a type as a resource
template <typename T> struct ResourceWrapper : public Resource {
  ResourceWrapper(T res) : resource(std::move(res)) {}

  T resource;
  void *getPtr() override { return &resource; }
};

/*template <typename T> struct ResourceTraits<ResourceWrapper<T>> {
  static std::unique_ptr<Resource> load(const char *id) {
    auto ptr = std::make_unique<TextureResource>();
    ptr->tex = loadTexture(id);
    return ptr;
  }

  static T *getPtr(Resource &res) {
    return &static_cast<ResourceWrapper<T>&>(res).resource;
  }
};*/

class AG_API ResourcePool {
public:
  Resource *getResource(const char *id);

  template <typename T, typename Traits = ResourceTraits<T>, typename... Args>
  T *get(const char *id, Args &&... loaderArgs) {
    auto pRes = getResource(id);
    if (!pRes) {
      auto res = Traits::load(id, std::forward<Args>(loaderArgs)...);
      pRes = res.get();
      // fill id and creation time
      pRes->id = id;
      pRes->lastReload = std::chrono::system_clock::now();
      resourceMap_[id] = std::move(res);
    }
    return Traits::getPtr(*pRes);
  }

  template <typename T, typename F> T *get_fn(const char *id, F loader) {
    auto pRes = getResource(id);
    if (!pRes) {
      auto res = loader(id);
      pRes = res.get();
      // fill id and creation time
      pRes->id = id;
      pRes->lastReload = std::chrono::system_clock::now();
      resourceMap_[id] = std::move(res);
    }
    return static_cast<T *>(pRes->getPtr());
  }

private:
  std::unordered_map<std::string, std::unique_ptr<Resource>> resourceMap_;
};

//
}
