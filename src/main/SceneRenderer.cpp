#include "SceneRenderer.h"
#include <autograph/support/ProjectRoot.h>
#include <autograph/support/Debug.h>
#include "Effect.h"

namespace ag 
{

    DeferredSceneRenderer::GBuffer DeferredSceneRenderer::GBuffer::create(ivec2 size)
    {
        GBuffer buf;
        buf.depthStencil = gl::Texture::create2D(ImageFormat::D32_SFLOAT);
        buf.diffuse = gl::Texture::create2D(ImageFormat::R8G8B8A8_UNORM);
        buf.normals = gl::Texture::create2D(ImageFormat::A2R10G10B10_UNORM_PACK32); // SNORM not supported in OpenGL
        buf.fbo = gl::Framebuffer::create({&buf.diffuse, &buf.normals}, buf.depthStencil);
    }
	
    DeferredSceneRenderer::SceneRenderer()
	{
        //deferredProgram_ = gl::Program::create(deferredShadingVertexShader, deferredShadingFragmentShader);
	}

    DeferredSceneRenderer::~DeferredSceneRenderer()
	{}

    void DeferredSceneRenderer::reloadShaders()
    {

    }

    void DeferredSceneRenderer::renderScene(GBuffer& targets, Scene& scene, Camera& camera)
	{
		auto& objects = scene.getObjects();
		for (auto&& obj : objects) 
		{
			AG_DEBUG("TODO: renderScene, object ID {} mesh {}", obj->id, (void*)obj->mesh);
		}
	}
}
