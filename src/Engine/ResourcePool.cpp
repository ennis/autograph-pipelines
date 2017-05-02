#include <autograph/Engine/ImageUtils.h>
#include <autograph/Engine/ResourcePool.h>

namespace ag {

Resource *ResourcePool::getResource(const char *id) {
  auto it = resourceMap_.find(id);
  if (it != resourceMap_.end()) {
    return it->second.get();
  } else {
    return nullptr;
  }
}


void Cache::addObject(std::shared_ptr<CacheObject> obj)
{
	cacheObjects_[obj->getPath()] = std::move(obj);
}

std::shared_ptr<CacheObject> Cache::getObject(const char *path)
{
	auto it = cacheObjects_.find(path);
	if (it != cacheObjects_.end()) {
		return it->second;
	}
	else {
		return nullptr;
	}
}

}
