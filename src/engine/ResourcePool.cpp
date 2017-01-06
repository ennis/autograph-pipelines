#include <autograph/engine/ResourcePool.h>
#include <autograph/engine/ImageUtils.h>

namespace ag
{

	Resource* ResourcePool::getResource(const char* id)
	{
		auto it = resourceMap_.find(id);
		if (it != resourceMap_.end()) {
			return it->second.get();
		}
		else 
		{
			return nullptr;
		}
	}

}

