#pragma once
#include <autograph/Transform.h>
#include "Mesh.h"
#include <vector>
#include <memory>

namespace ag 
{
	struct SceneObject 
	{
		uint64_t id;
		Mesh* mesh;
		Transform transform;
	};

	class Scene 
	{
	public:
		void clear();

		SceneObject& addMesh(Mesh& mesh);
		SceneObject& loadMesh(std::string path);

		auto& getObjects() {
			return sceneObjects_;
		}

	private:
		std::vector<std::unique_ptr<SceneObject>> sceneObjects_;
		std::vector<std::unique_ptr<Mesh>> ownedMeshes_;
	};
}