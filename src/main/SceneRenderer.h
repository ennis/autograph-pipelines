#include "Scene.h"
#include "ShaderManager.h"
#include "Effect.h"
#include <autograph/gl/Framebuffer.h>
#include <autograph/gl/Program.h>
#include <autograph/gl/Texture.h>
#include <autograph/gl/UploadBuffer.h>
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
            GBuffer(int width, int height);

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

        DeferredSceneRenderer(ShaderManager& sm);
        ~DeferredSceneRenderer();

        void reloadShaders(ShaderManager& sm);
        void renderScene(GBuffer& targets, Scene& scene, Camera& camera);

	private:
        std::unique_ptr<DrawPass> deferredDrawPass;
	};

	class WireframeOverlayRenderer
	{
	public:
		WireframeOverlayRenderer(ShaderManager& sm);
		~WireframeOverlayRenderer();

		void reloadShaders(ShaderManager& sm);
		void renderSceneObject(gl::Framebuffer& target, Scene& scene, SceneObject& object, Camera& camera, bool depthTest = true);
	private:
		std::unique_ptr<DrawPass> wireframeDrawPass;
		std::unique_ptr<DrawPass> wireframeNoDepthDrawPass;
	};
}
