#pragma once
#include <autograph/Config.h>
#include <chrono>
#include <unordered_map>
#include <memory>

namespace ag {
struct Resource {
  virtual ~Resource() {}

  std::string id;
  std::chrono::system_clock::time_point lastReload;
};

template <typename T> struct ResourceTraits;

class AG_API ResourcePool {
public:
  Resource *getResource(const char *id);

  template <typename T> T *get(const char *id) {
    auto pRes = getResource(id);
    if (!pRes) {
      auto res = ResourceTraits<T>::load(id);
      pRes = res.get();
      // fill id and creation time
      pRes->id = id;
      pRes->lastReload = std::chrono::system_clock::now();
      resourceMap_[id] = std::move(res);
    }
    return ResourceTraits<T>::getPtr(*pRes);
  }

private:
  std::unordered_map<std::string, std::unique_ptr<Resource>> resourceMap_;
};

//
}