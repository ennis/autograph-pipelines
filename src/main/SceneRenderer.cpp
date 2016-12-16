#include "SceneRenderer.h"
#include <autograph/support/ProjectRoot.h>
#include <autograph/support/Debug.h>
#include "Effect.h"

namespace ag 
{

    DeferredSceneRenderer::GBuffer::GBuffer(ivec2 size)
    {
        depthStencil = gl::Texture::create2D(ImageFormat::D32_SFLOAT, size.x, size.y);
        diffuse = gl::Texture::create2D(ImageFormat::R8G8B8A8_UNORM, size.x, size.y);
        normals = gl::Texture::create2D(ImageFormat::A2R10G10B10_UNORM_PACK32, size.x, size.y); // SNORM not supported in OpenGL
        fbo = gl::Framebuffer::create({&diffuse, &normals}, depthStencil);
    }
	
    DeferredSceneRenderer::DeferredSceneRenderer()
	{
	}

    DeferredSceneRenderer::~DeferredSceneRenderer()
	{}

    void DeferredSceneRenderer::reloadShaders()
    {
        AG_DEBUG("DeferredSceneRenderer::reloadShaders");
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
