#pragma once
#include <autograph/Camera.h>
#include <autograph/Rect.h>
#include <autograph/engine/Mesh.h>
#include <autograph/engine/RenderTarget.h>
#include <autograph/engine/Scene.h>
#include <autograph/engine/Shader.h>
#include <autograph/gl/Sampler.h>

namespace ag {
class AG_API RenderUtils {
public:
  RenderUtils();
  void reloadShaders();

  // 0,0 is top left
  void drawSprite(gl::Framebuffer &target, float targetX, float targetY,
                  float targetWidth, float targetHeight, gl::Texture &src,
                  float srcX = 0.0f, float srcY = 0.0f, float srcWidth = 1.0f,
                  float srcHeight = 1.0f);
  void drawSprite(gl::Framebuffer &target, float targetX, float targetY,
	  float targetWidth, float targetHeight, GLuint tex,
	  float srcX = 0.0f, float srcY = 0.0f, float srcWidth = 1.0f,
	  float srcHeight = 1.0f);

  // Draw mesh with default view-dependent shading
  void drawMesh(gl::Framebuffer &target, const Camera &cam, Mesh3D &mesh,
                vec3 pos, float scale = 1.0f,
                vec4 color = vec4{0.9f, 1.0f, 0.9f, 1.0f});
  void drawMesh(gl::Framebuffer &target, const Camera &cam, Mesh3D &mesh,
                mat4 modelTransform, vec4 color = vec4{0.9f, 1.0f, 0.9f, 1.0f});
  // Draw mesh in wireframe
  void drawWireMesh(gl::Framebuffer &target, const Camera &cam, Mesh3D &mesh,
                    vec3 pos, float scale = 1.0f,
                    vec4 wireColor = vec4{0.0f, 1.0f, 0.0f, 1.0f});
  void drawWireMesh(gl::Framebuffer &target, const Camera &cam, Mesh3D &mesh,
                    mat4 modelTransform,
                    vec4 wireColor = vec4{0.0f, 1.0f, 0.0f, 1.0f});
  // Draw a bounding box
  void drawBoundingBox(gl::Framebuffer &target, const Camera &cam,
	  const AABB &aabb,
	  vec4 wireColor = vec4{ 1.0f, 1.0f, 0.0f, 1.0f });
  // Draw a 2D grid
  void drawGrid2D(gl::Framebuffer &target,
	  vec2 center, // screen coords
	  vec2 spacing, // screen coords
	  int primaryLinesEveryN,
	  vec4 axisColorX = vec4{ 1.0f, 0.0f, 0.0f, 1.0f },
	  vec4 axisColorY = vec4{ 0.0f, 1.0f, 0.0f, 1.0f },
	  vec4 primaryLinesColor = vec4{ 0.85f, 0.85f, 0.85f, 1.0f },
	  vec4 secondaryLinesColor = vec4{ 0.2f, 0.2f, 0.2f, 1.0f });

  gl::Sampler samplerNearest;
  gl::Sampler samplerLinear;

private:
  Shader drawSpriteShader;
  Shader drawMeshShader;
  Shader drawWireMeshShader;
  Shader drawWireMeshNoDepthShader;
  Shader drawWireMesh2DColorShader;
};

// get shared instance
AG_API RenderUtils &getRenderUtils();
}