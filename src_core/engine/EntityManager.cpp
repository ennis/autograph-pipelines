#include <autograph/engine/EntityManager.h>

namespace ag 
{

Scene::Scene(EntityManager& entityManager) : entityManager_{entityManager}
{
}

void Scene::registerComponentManager(ComponentManagerBase& componentManager)
{
	componentManagers_.insert(&componentManager);
}

void Scene::unregisterComponentManager(ComponentManagerBase& componentManager)
{
	componentManagers_.erase(&componentManager);
}

void Scene::showGUI(ID id)
{
	for (auto& man : componentManagers_) {
		man->showGUI(id);
	}
}

ComponentManagerBase* Scene::getComponentManager(std::type_index ti)
{
	for (auto man : componentManagers_) {
		if (ti == typeid(*man)) {
			return man;
		}
	}
	return nullptr;
}

}