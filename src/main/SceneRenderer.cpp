#include "SceneRenderer.h"
#include <autograph/support/ProjectRoot.h>
#include <autograph/support/Debug.h>
#include <autograph/gl/Draw.h>
#include <autograph/gl/Device.h>
#include "Effect.h"

namespace ag 
{
    DeferredSceneRenderer::GBuffer::GBuffer(int width, int height)
    {
        depthStencil = gl::Texture::create2D(ImageFormat::D32_SFLOAT, width, height);
        diffuse = gl::Texture::create2D(ImageFormat::R8G8B8A8_UNORM, width, height);
        normals = gl::Texture::create2D(ImageFormat::A2R10G10B10_UNORM_PACK32, width, height); // SNORM not supported in OpenGL
        fbo = gl::Framebuffer::create({&diffuse, &normals}, depthStencil);
    }
	
    DeferredSceneRenderer::DeferredSceneRenderer(ShaderManager& sm)
	{
		reloadShaders(sm);
	}

    DeferredSceneRenderer::~DeferredSceneRenderer()
	{}

    void DeferredSceneRenderer::reloadShaders(ShaderManager& sm)
    {
        AG_DEBUG("DeferredSceneRenderer::reloadShaders");
		sm.loadShaderFile("deferred");
		deferredDrawPass = sm.createDrawPass("deferredShader");
    }

    void DeferredSceneRenderer::renderScene(GBuffer& targets, Scene& scene, Camera& camera)
	{
		// Per-object uniforms
		struct ObjectUniforms
		{
			mat4 modelMatrix;
			mat4 viewMatrix;
			mat4 projMatrix;
			mat4 viewProjMatrix;
		} objectUniforms;

		auto& objects = scene.getObjects();
		for (auto&& obj : objects) 
		{
			using namespace gl;
			using namespace gl::bind;
			auto pMesh = obj->mesh;
			auto vtxCount = pMesh->getVertices().size();
			auto idxCount = pMesh->getIndices().size();
			objectUniforms.modelMatrix = obj->transform.getMatrix();
			objectUniforms.viewMatrix = camera.viewMat;
			objectUniforms.projMatrix = camera.projMat;
			objectUniforms.viewProjMatrix = camera.projMat * camera.viewMat;
			draw(
				gl::getDefaultFramebuffer(),
				drawIndexed(GL_TRIANGLES, 0, idxCount, 0),
				deferredDrawPass->getDrawStates(),
				vertexBuffer(0, pMesh->getVertexBuffer(), sizeof(Vertex3D)),
				indexBuffer(pMesh->getIndexBuffer(), GL_UNSIGNED_INT),
				uniformBuffer(0, uploadFrameData(&objectUniforms, sizeof(objectUniforms), alignof(ObjectUniforms)))
				);

			AG_DEBUG("renderScene, object ID {} mesh {}", obj->id, (void*)obj->mesh);
		}
	}
}
