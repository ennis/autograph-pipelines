#include "Scene.h"
#include <autograph/gl/Framebuffer.h>
#include <autograph/gl/Program.h>
#include <autograph/Camera.h>

namespace ag 
{
	// scene renderer
	// Renders G-buffers
	class SceneRenderer 
	{
	public:
		SceneRenderer();
		~SceneRenderer();

		void renderScene(gl::Framebuffer& target, Scene& scene, Camera& camera);

	private:
		gl::Program deferredProgram_;
	};
}