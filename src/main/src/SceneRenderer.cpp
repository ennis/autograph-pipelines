#include "SceneRenderer.h"
#include <autograph/support/ProjectRoot.h>
#include <autograph/support/Debug.h>

namespace ag 
{
	auto deferredShadingVertexShader = R"(
layout(std140, binding = 0) uniform U0 {
  mat4 modelMatrix;
  mat4 viewMatrix;
  mat4 projMatrix;
  mat4 viewProjMatrix;
};

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent;
layout(location = 3) in vec2 texcoords;
out vec3 Nw0;
out vec3 Pv;
out vec3 Tv0;
out vec2 fTexcoords;
void main() {
  gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(position, 1.0f);
  // assume no scaling in modelMatrix
  Nw0 = (modelMatrix * vec4(normal, 0.0)).xyz;
  Tv0 = (viewMatrix * modelMatrix * vec4(tangent, 0.0)).xyz;
  Pv = (viewMatrix * modelMatrix * vec4(position, 1.0)).xyz;
  fTexcoords = texcoords;
}
	)";

	auto deferredShadingFragmentShader = R"(
in vec3 Nw0;
in vec3 Pv;
in vec3 Tv0;
in vec2 fTexcoords;

layout(location = 0) out vec4 rtStencil;
layout(location = 1) out vec4 rtPositions;
layout(location = 2) out vec4 rtNormals;
layout(location = 3) out vec4 rtDepth;
layout(location = 4) out vec4 rtTangents;

void main() {
  rtPositions = vec4(Pv, 1.0f);
  rtNormals = vec4(Nw0 / 2.0 + vec3(0.5), 1.0f);
  rtStencil = vec4(1.0);
}
	)";
	
	SceneRenderer::SceneRenderer()
	{
		deferredProgram_ = gl::Program::create(deferredShadingVertexShader, deferredShadingFragmentShader);
	}

	SceneRenderer::~SceneRenderer()
	{}

	void SceneRenderer::renderScene(gl::Framebuffer& target, Scene& scene, Camera& camera)
	{
		auto& objects = scene.getSceneObjects();
		for (auto&& obj : objects) 
		{
			AG_DEBUG("TODO: renderScene, object ID {} mesh {}", obj->id, (void*)obj->mesh);
		}
	}
}