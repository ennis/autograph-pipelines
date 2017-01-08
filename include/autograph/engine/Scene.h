#pragma once
#include <autograph/Transform.h>
#include "Mesh.h"
#include <vector>
#include <memory>

struct aiScene;
struct aiMesh;
struct aiNode;

namespace ag 
{
	struct SceneObject 
	{
		SceneObject* parent{ nullptr };
		uint64_t id{ 0 };
		Mesh3D* mesh{ nullptr };
		Transform localTransform;
		mat4 worldTransform;
		std::vector<SceneObject*> children;
		AABB worldBounds;
		AABB localBounds;
		bool hasWorldBounds{ false };

		// includes children
		AABB getLocalBoundingBox() const;
		AABB getApproximateWorldBoundingBox() const;
	};

	class Scene 
	{
	public:
		Scene();
		void clear();

		auto addObject()->SceneObject&;
		auto addMesh(Mesh3D& mesh) -> SceneObject&;
		auto loadModel(const char* path) -> SceneObject&;
		void update();

		auto& getObjects() {
			return sceneObjects_;
		}

	private:
		void updateWorldTransformsRecursive(mat4 current, SceneObject& obj);
		void updateObjectBoundsRecursive(SceneObject& obj);
		auto importAssimpMesh(const aiScene* scene, int index, Mesh3D** loadedMeshes) ->Mesh3D*;
		auto importAssimpNodeRecursive(const aiScene* scene, aiNode* node, SceneObject* parent, Mesh3D** loadedMeshes)->SceneObject*;
		auto makeOwnedMesh(std::unique_ptr<Mesh3D> pMesh)->Mesh3D*;
		SceneObject* rootObj_;
		std::vector<std::unique_ptr<SceneObject>> sceneObjects_;
		std::vector<std::unique_ptr<Mesh3D>> ownedMeshes_;
	};
}