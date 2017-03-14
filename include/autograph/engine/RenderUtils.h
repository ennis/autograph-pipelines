#pragma once
#include <autograph/engine/Config.h>
#include <autograph/Camera.h>
#include <autograph/Rect.h>
#include <autograph/engine/Mesh.h>
//#include <autograph/engine/RenderTarget.h>
#include <autograph/gl/Sampler.h>

namespace ag {

struct SceneObject;
class Shader;

namespace RenderUtils {
AG_ENGINE_API void reloadShaders();

// 0,0 is top left
// TODO rename to drawTexturedQuad?
AG_ENGINE_API void drawSprite(gl::Framebuffer &target, float targetX, float targetY,
                       float targetWidth, float targetHeight, gl::Texture &src,
                       float srcX = 0.0f, float srcY = 0.0f,
                       float srcWidth = 1.0f, float srcHeight = 1.0f);
AG_ENGINE_API void drawSprite(gl::Framebuffer &target, float targetX, float targetY,
                       float targetWidth, float targetHeight, GLuint tex,
                       float srcX = 0.0f, float srcY = 0.0f,
                       float srcWidth = 1.0f, float srcHeight = 1.0f);
AG_ENGINE_API void drawTexturedQuad(gl::Framebuffer &target, gl::Texture &src);

// Draw mesh with default view-dependent shading
AG_ENGINE_API void drawMesh(gl::Framebuffer &target, const Camera &cam, Mesh3D &mesh,
                     vec3 pos, float scale = 1.0f,
                     vec4 color = vec4{0.9f, 1.0f, 0.9f, 1.0f});
AG_ENGINE_API void drawMesh(gl::Framebuffer &target, const Camera &cam, Mesh3D &mesh,
                     mat4 modelTransform,
                     vec4 color = vec4{0.9f, 1.0f, 0.9f, 1.0f});
// Draw mesh in wireframe
AG_ENGINE_API void drawWireMesh(gl::Framebuffer &target, const Camera &cam,
                         Mesh3D &mesh, vec3 pos, float scale = 1.0f,
                         vec4 wireColor = vec4{0.0f, 1.0f, 0.0f, 1.0f});
AG_ENGINE_API void drawWireMesh(gl::Framebuffer &target, const Camera &cam,
                         Mesh3D &mesh, mat4 modelTransform,
                         vec4 wireColor = vec4{0.0f, 1.0f, 0.0f, 1.0f});
AG_ENGINE_API void drawLines(gl::Framebuffer &target, const Camera &cam,
	span<const Vertex3D> lines, mat4 modelTransform = mat4{ 1.0f },
	float lineWidth = 1.0f,
	vec4 wireColor = vec4{ 0.0f, 1.0f, 0.0f, 1.0f });

// Draw a bounding box
AG_ENGINE_API void drawBoundingBox(gl::Framebuffer &target, const Camera &cam,
                            const AABB &aabb,
                            vec4 wireColor = vec4{1.0f, 1.0f, 0.0f, 1.0f});
// Draw a 2D grid
AG_ENGINE_API void drawGrid2D(gl::Framebuffer &target,
                       vec2 center,  // screen coords
                       vec2 spacing, // screen coords
                       int primaryLinesEveryN,
                       vec4 axisColorX = vec4{1.0f, 0.0f, 0.0f, 1.0f},
                       vec4 axisColorY = vec4{0.0f, 1.0f, 0.0f, 1.0f},
                       vec4 primaryLinesColor = vec4{0.85f, 0.85f, 0.85f, 1.0f},
                       vec4 secondaryLinesColor = vec4{0.2f, 0.2f, 0.2f, 1.0f});

// TODO make this more generic
AG_ENGINE_API gl::Sampler& getLinearSampler();
AG_ENGINE_API gl::Sampler& getNearestSampler();
}
}