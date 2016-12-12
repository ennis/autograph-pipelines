#include "Scene.h"

namespace ag {

	void Scene::clear()
	{
		sceneObjects_.clear();
	}

	SceneObject &Scene::addMesh(Mesh &mesh)
	{
		auto obj = std::make_unique<SceneObject>();
		obj->mesh = &mesh;
		obj->id = 0;
		auto ptr = obj.get();
		sceneObjects_.push_back(std::move(obj));
		return *ptr;
	}

	SceneObject &Scene::loadMesh(std::string id)
	{
		auto mesh = std::make_unique<Mesh>(Mesh::loadFromFile(id.c_str()));
		auto& sceneobj = addMesh(*mesh);
		ownedMeshes_.push_back(std::move(mesh));
		return sceneobj;
	}

}