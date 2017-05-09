#pragma once
#include <autograph/Engine/Exports.h>
#include <autograph/Engine/Plugin.h>
#include <autograph/Engine/EntityManager.h>

namespace ag {
	class Scene;
	struct Camera;
	class ResourcePool;

	class SceneEditor : public Extension
	{
	public:
		virtual void onSceneEditorGUI(Scene& s, ID& lastSelected, const Camera& camera, ResourcePool& resourcePool) = 0;
	};
}