#pragma once
#include <autograph/Core/Exports.h>
#include <chrono>
#include <memory>
#include <string>
#include <unordered_map>

namespace ag {

///
/// CacheObject
class CacheObject {
public:
  CacheObject(const char *path_) : path{path_} {}
  virtual ~CacheObject() {}
  virtual void reload() {}
  const char *getPath() const { return path.c_str(); }

protected:
  std::string path;
};

///
/// Cache
class AG_CORE_API Cache {
public:
  void addObject(std::shared_ptr<CacheObject> obj);
  std::shared_ptr<CacheObject> getObject(const char *path) const;

  template <typename T>
  std::shared_ptr<T> getObjectOfType(const char *path) const {
    static_assert(std::is_base_of<CacheObject, T>::value,
                  "T must be a derived class of CacheObject");
    if (auto obj = getObject(path)) {
      return std::dynamic_pointer_cast<T>(obj);
    }
    return nullptr;
  }

  static Cache &getDefault();

private:
  std::unordered_map<std::string, std::shared_ptr<CacheObject>> cacheObjects_;
};

//////////////////////////////////////////////
// CachedResource helper
template <typename T> struct CachedResource : public CacheObject {
	CachedResource(const char* path, T &&res) : CacheObject{ path }, resource{ std::move(res) } {}
	T resource;
};

template <typename T>
std::shared_ptr<T> getCachedResource(Cache &cache_, const char *path) {
  auto ptr = cache_.getObjectOfType<CachedResource<T>>(path);
  if (!ptr) return nullptr;
  auto presource = &ptr->resource;
  return std::shared_ptr<T>{std::move(ptr), presource};
}

template <typename T> std::shared_ptr<T> addCachedResource(Cache &cache, const char* path, T &&resource) {
  auto res = std::make_shared<CachedResource<T>>(path, std::move(resource));
  cache.addObject(res);
  auto presource = &res->resource;
  return std::shared_ptr<T>{std::move(res), presource};
}

} // namespace ag
