#pragma once
#include <autograph/Engine/RenderTarget.h>
#include <autograph/Engine/SceneObject.h>
#include <autograph/Engine/Shader.h>
#include <functional>
#include <vector>

namespace ag {

/////////////////////////////////////////////////////////////
// Converts a sequence of mouse position samples to
// a sequence of splat positions
// Also handles smoothing
class BrushPathBuilder {
public:
  struct Params {
    float spacing;
    float spacingJitter;
  };

  struct Point {
    bool first;
    float x;
    float y;
    float prevX;
    float prevY;
  };

  // call this when the mouse has moved
  // returns how many splats were added as a result of the mouse movement
  int addPoint(const Params &params, float x, float y,
               std::function<void(Point)> f);

  void clear();

private:
  std::vector<vec2> pointerPositions_;
  std::vector<vec2> splatPositions_;
  float pathLength_{0.0f};
};

// Augmented painting canvas
struct Canvas {
  Canvas() = default;
  Canvas(int width_, int height_);
  RenderTarget GBuffers;
  RenderTarget canvasBuffers;
  RenderTarget finalColor;
  int width;
  int height;
};

class CanvasRenderer {
public:
  CanvasRenderer();
  void renderCanvas(SceneObjectComponents &scene, Canvas &canvas);
  void reloadShaders();

private:
  GPUPipeline gbufferPass;
  GPUPipeline evaluationPass;
};
}
