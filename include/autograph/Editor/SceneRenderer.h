#pragma once
#include <autograph/Engine/All.h>
#include <autograph/Gfx/All.h>

namespace ag {
class SceneRenderer : public Extension {
public:
  enum class DebugRenderMode {
    None = 0,
    Normals = 1,
    ObjectID = 2,
    Depth = 3,
    Positions = 4,
    Albedo = 5,
    Velocity = 6,
  };

  virtual void resize(int width, int height) = 0;
  virtual void
  renderScene(Framebuffer &target, SceneObjectComponents &sceneObjects,
              RenderableComponents &renderables, LightComponents &lights,
              const Camera &camera,
              DebugRenderMode debugRenderMode = DebugRenderMode::None) = 0;

private:
};
}
