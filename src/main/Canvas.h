#pragma once
#include <autograph/Config.h>
#include <autograph/engine/RenderTarget.h>
#include <autograph/engine/Scene.h>
#include <autograph/engine/Shader.h>
#include <vector>
#include <functional>

namespace ag 
{

/////////////////////////////////////////////////////////////
// Converts a sequence of mouse position samples to
// a sequence of splat positions
// Also handles smoothing
class AG_API BrushPathBuilder {
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

	void renderCanvas(Scene& scene, Canvas &canvas);

	void reloadShaders();

private:
	gl::Sampler sampler;
	Shader gbufferPass;
	Shader evaluationPass;
};

}