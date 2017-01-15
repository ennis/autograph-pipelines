#pragma once
#include <autograph/engine/Scene.h>
#include <autograph/engine/Shader.h>
#include <autograph/Rect.h>
#include <autograph/gl/Sampler.h>

namespace ag
{
	class RenderUtils
	{
	public:
		RenderUtils();
		void reloadShaders();

		// 0,0 is top left
		void drawSprite(
			gl::Framebuffer& target, float targetX, float targetY, float targetWidth, float targetHeight, 
			gl::Texture& src, float srcX = 0.0f, float srcY = 0.0f, float srcWidth = 1.0f, float srcHeight = 1.0f);
		
	private:
		Shader drawSpriteShader;
		gl::Sampler samplerNearest;
	};
}