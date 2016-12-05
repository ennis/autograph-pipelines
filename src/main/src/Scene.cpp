#include "Scene.h"

namespace ag {

	void Scene::clear()
	{
		sceneObjects_.clear();
	}

	SceneObject &Scene::createSceneObject(Mesh &mesh)
{
	auto obj = std::make_unique<SceneObject>();
	obj->mesh = &mesh;
	obj->id = 0;
	auto ptr = obj.get();
	sceneObjects_.push_back(std::move(obj));
	return *ptr;
}

}