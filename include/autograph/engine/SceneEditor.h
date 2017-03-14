#pragma once
#include <autograph/engine/Config.h>

namespace ag {
struct Camera;
class SceneObjectComponents;
class Scene;
class RenderableComponents;
class LightComponents;
class ResourcePool;
class EntityManager;

namespace SceneEditor {
AG_ENGINE_API void show(const Camera &cam, EntityManager &entityManager, SceneObjectComponents &scene, 
		  Scene& genericScene,
          RenderableComponents &renderableScene, LightComponents &lights,
          ResourcePool &resourcePool);
}
}