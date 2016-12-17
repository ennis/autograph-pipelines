#include "Scene.h"
#include "Effect.h"
#include <autograph/gl/Framebuffer.h>
#include <autograph/gl/Program.h>
#include <autograph/gl/Texture.h>
#include <autograph/Camera.h>

namespace ag 
{
	// scene renderer
	// Renders G-buffers
    class DeferredSceneRenderer
	{
	public:
        struct GBuffer
        {
        public:
            GBuffer() {}
            GBuffer(ivec2 size);

            gl::Texture& getDepthTarget() {
                return depthStencil;
            }

            gl::Texture& getNormalsTarget() {
                return normals;
            }

            gl::Texture& getDiffuseTarget() {
                return diffuse;
            }

            gl::Framebuffer& getFramebuffer() {
                return fbo;
            }

        private:
            gl::Texture depthStencil; // D32_SFLOAT
            gl::Texture normals; // R10G10B10A2
            gl::Texture diffuse; // R8G8B8A8
            gl::Framebuffer fbo;
        };

        DeferredSceneRenderer();
        ~DeferredSceneRenderer();

        void reloadShaders();
        void renderScene(GBuffer& targets, Scene& scene, Camera& camera);

	private:
        std::unique_ptr<DrawPass> deferredDrawPass;
	};
}
