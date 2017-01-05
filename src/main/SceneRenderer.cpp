#include "SceneRenderer.h"
#include <autograph/support/ProjectRoot.h>
#include <autograph/support/Debug.h>
#include <autograph/gl/Draw.h>
#include <autograph/gl/Device.h>
#include <autograph/engine/Shader.h>

namespace ag 
{
    DeferredSceneRenderer::GBuffer::GBuffer(int width, int height)
    {
        depthStencil = gl::Texture::create2D(ImageFormat::D32_SFLOAT, width, height);
        diffuse = gl::Texture::create2D(ImageFormat::R8G8B8A8_UNORM, width, height);
        normals = gl::Texture::create2D(ImageFormat::A2R10G10B10_UNORM_PACK32, width, height); // SNORM not supported in OpenGL
		fbo.setAttachement(GL_COLOR_ATTACHMENT0 + 0, diffuse);
		fbo.setAttachement(GL_COLOR_ATTACHMENT0 + 1, normals);
		fbo.setAttachement(GL_DEPTH_ATTACHMENT, depthStencil);
    }

	void DeferredSceneRenderer::GBuffer::release()
	{
		depthStencil = {};
		diffuse = {};
		normals = {};
		fbo = {};
	}
	
    DeferredSceneRenderer::DeferredSceneRenderer()
	{
		reloadShaders();
	}

    DeferredSceneRenderer::~DeferredSceneRenderer()
	{}

    void DeferredSceneRenderer::reloadShaders()
    {
		deferredDrawPass = DrawPass{ "shaders/deferred:deferredShader" };
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

		gl::clearTexture(targets.getDiffuseTarget(), vec4{ 0.0f,0.0f,0.0f,1.0f });
		gl::clearDepthTexture(targets.getDepthTarget(), 1.0f);

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
				targets.getFramebuffer(),
				drawIndexed(GL_TRIANGLES, 0, idxCount, 0),
				deferredDrawPass.getDrawStates(),
				vertexBuffer(0, pMesh->getVertexBuffer(), sizeof(Vertex3D)),
				indexBuffer(pMesh->getIndexBuffer(), GL_UNSIGNED_INT),
                uniformBuffer(0, uploadFrameData(&objectUniforms, sizeof(objectUniforms)))
				);

			//AG_DEBUG("renderScene, object ID {} mesh {}", obj->id, (void*)obj->mesh);
		}
	}


	WireframeOverlayRenderer::WireframeOverlayRenderer()
	{
		reloadShaders();
	}

	WireframeOverlayRenderer::~WireframeOverlayRenderer()
	{}

	void WireframeOverlayRenderer::reloadShaders()
	{
		wireframeDrawPass = DrawPass{ "shaders/wireframe:wireframeOverlay" };
		wireframeNoDepthDrawPass = DrawPass{ "shaders/wireframe:wireframeOverlayNoDepth" };
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
				depthTest ? wireframeDrawPass.getDrawStates() : wireframeNoDepthDrawPass.getDrawStates(),
				vertexBuffer(0, pMesh->getVertexBuffer(), sizeof(Vertex3D)),
				indexBuffer(pMesh->getIndexBuffer(), GL_UNSIGNED_INT),
				uniformBuffer(0, uploadFrameData(&objectUniforms, sizeof(objectUniforms)))
			);
		}
		for (auto c : object.children) {
			renderSceneObject(target, scene, *c, camera, depthTest);
		}

	}


}

