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

// wrap a type as a resource
template <typename T> struct ResourceWrapper
{
	T resource;
	operator T*() {
		return &resource;
	}
};

class AG_API ResourcePool {
public:
  Resource *getResource(const char *id);

  template <typename T, typename Traits = ResourceTraits<T>, typename... Args> T *get(const char *id, Args&&... loaderArgs) {
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
	  return (T*)(*pRes);
  }




private:
  std::unordered_map<std::string, std::unique_ptr<Resource>> resourceMap_;
};

//
}