// Shared ImGui utilities
#pragma once
#include <autograph/Camera.h>
#include <autograph/engine/Scene.h>
#include <autograph/engine/RenderableScene.h>
#include <autograph/engine/ResourcePool.h>
#include <autograph/gl/All.h>

namespace ag {
namespace gui {
void beginFrame();
void sceneEditor(const Camera &cam, EntityManager& entityManager, Scene &scene, RenderableScene& renderableScene, ResourcePool& resourcePool, ID rootEntityID);
void endFrame();
}
}