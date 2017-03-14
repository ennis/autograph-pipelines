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

void Scene::showGUI(ID id)
{
	// TODO
	for (auto& man : componentManagers_) {
		man->showGUI(id);
	}
}

}