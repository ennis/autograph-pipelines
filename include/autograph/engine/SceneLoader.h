#include <autograph/engine/SceneObject.h>
#include <autograph/engine/Light.h>
#include <autograph/engine/Renderable.h>
#include <autograph/engine/EntityManager.h>
#include <autograph/engine/ResourcePool.h>
#include <autograph/support/Utils.h>

namespace ag {

/*enum class LoadOptions {
  LoadMeshes = (1 << 0),
  LoadMaterials = (1 << 1),
  LoadPhysics = (1 << 2)
};

ENUM_BIT_FLAGS_OPERATORS(LoadOptions);*/

// Loads a scene from a file into an existing scene
AG_ENGINE_API ID loadScene(const char *id,
	EntityManager& entities, 
	SceneObjectComponents &scene,		// meshes, transform hierarchy
	RenderableComponents& renderableScene,	// renderables
	LightComponents& lights, // lights
	ResourcePool &resourcePool,
	SceneObject* parentObject = nullptr);


}
