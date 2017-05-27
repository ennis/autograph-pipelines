#include <autograph/Core/Cache.h>

namespace ag {

void Cache::addObject(std::shared_ptr<CacheObject> obj) {
  cacheObjects_[obj->getPath()] = std::move(obj);
}

std::shared_ptr<CacheObject> Cache::getObject(const char *path) const {
  auto it = cacheObjects_.find(path);
  if (it != cacheObjects_.end()) {
    return it->second;
  } else {
    return nullptr;
  }
}

Cache& Cache::getDefault() {
	static Cache cache;
	return cache; 
}

}
