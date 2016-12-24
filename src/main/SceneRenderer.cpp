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
			auto pMesh = obj->mesh;
			if (!pMesh) continue;
			using namespace gl;
			using namespace gl::bind;
			auto vtxCount = pMesh->getVerticesCount();
			auto idxCount = pMesh->getIndicesCount();
			objectUniforms.modelMatrix = obj->worldTransform;
			objectUniforms.viewMatrix = camera.viewMat;
			objectUniforms.projMatrix = camera.projMat;
			objectUniforms.viewProjMatrix = camera.projMat * camera.viewMat;

			//AG_DEBUG("modelMat {},{},{},{}", objectUniforms.modelMatrix[0], objectUniforms.modelMatrix[1], objectUniforms.modelMatrix[2], objectUniforms.modelMatrix[3]);
			//AG_DEBUG("viewMat {},{},{},{}", camera.viewMat[0], camera.viewMat[1], camera.viewMat[2], camera.viewMat[3]);
			//AG_DEBUG("projMat {},{},{},{}", camera.projMat[0], camera.projMat[1], camera.projMat[2], camera.projMat[3]);
			draw(
				gl::getDefaultFramebuffer(),
				drawIndexed(GL_TRIANGLES, 0, idxCount, 0),
				deferredDrawPass->getDrawStates(),
				vertexBuffer(0, pMesh->getVertexBuffer(), sizeof(Vertex3D)),
				indexBuffer(pMesh->getIndexBuffer(), GL_UNSIGNED_INT),
				uniformBuffer(0, uploadFrameData(&objectUniforms, sizeof(objectUniforms), 128))
				);

			//AG_DEBUG("renderScene, object ID {} mesh {}", obj->id, (void*)obj->mesh);
		}
	}


	WireframeOverlayRenderer::WireframeOverlayRenderer(ShaderManager& sm)
	{
		reloadShaders(sm);
	}

	WireframeOverlayRenderer::~WireframeOverlayRenderer()
	{}

	void WireframeOverlayRenderer::reloadShaders(ShaderManager& sm)
	{
		AG_DEBUG("WireframeOverlayRenderer::reloadShaders");
		sm.loadShaderFile("wireframe");
		wireframeDrawPass = sm.createDrawPass("wireframeOverlay");
		wireframeNoDepthDrawPass = sm.createDrawPass("wireframeOverlayNoDepth");
	}

	// render one scene object and its children
	void WireframeOverlayRenderer::renderSceneObject(gl::Framebuffer& target, Scene& scene, SceneObject& object, Camera& camera, bool depthTest)
	{
		auto pMesh = object.mesh;
		
		if (pMesh) {
			// Per-object uniforms
			struct ObjectUniforms
			{
				mat4 modelMatrix;
				mat4 viewMatrix;
				mat4 projMatrix;
				mat4 viewProjMatrix;
			} objectUniforms;

			objectUniforms.modelMatrix = object.worldTransform;
			objectUniforms.viewMatrix = camera.viewMat;
			objectUniforms.projMatrix = camera.projMat;
			objectUniforms.viewProjMatrix = camera.projMat * camera.viewMat;

			using namespace gl;
			using namespace gl::bind;
			auto vtxCount = pMesh->getVerticesCount();
			auto idxCount = pMesh->getIndicesCount();

			draw(
				target,
				drawIndexed(GL_TRIANGLES, 0, idxCount, 0),
				depthTest ? wireframeDrawPass->getDrawStates() : wireframeNoDepthDrawPass->getDrawStates(),
				vertexBuffer(0, pMesh->getVertexBuffer(), sizeof(Vertex3D)),
				indexBuffer(pMesh->getIndexBuffer(), GL_UNSIGNED_INT),
				uniformBuffer(0, uploadFrameData(&objectUniforms, sizeof(objectUniforms), 128))
			);
		}
		for (auto c : object.children) {
			renderSceneObject(target, scene, *c, camera, depthTest);
		}

	}


}

