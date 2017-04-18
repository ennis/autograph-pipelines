#pragma once
#include <autograph/engine/Config.h>
#include <autograph/engine/Plugin.h>
#include <autograph/engine/EntityManager.h>

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