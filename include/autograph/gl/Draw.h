#pragma once
#include <autograph/gl/Bind.h>

namespace ag
{
	namespace gl
	{

		inline auto drawArrays(
			GLenum primitiveType,
			uint32_t first,
			uint32_t count)
		{
			return [=](StateGroup& sg) {
				glDrawArrays(primitiveType, first, count);
			};
		}

		inline auto drawIndexed(
			GLenum primitiveType,
			uint32_t first,
			uint32_t count,
			uint32_t baseVertex)
		{
			return [=](StateGroup& sg)
			{
				auto indexStride = (sg.uniforms.indexBufferType == GL_UNSIGNED_SHORT) ? 2 : 4;
				glDrawElementsBaseVertex(
					primitiveType, count, sg.uniforms.indexBufferType,
					((const char*)((uintptr_t)first * indexStride)), baseVertex);
			};
		}


		////////////////////////// draw

		template <typename DrawCommand, typename... Arguments>
		void draw(
			Framebuffer& fbo,
			const DrawCommand& drawCommand,
			const DrawStates& drawStates,
			Arguments&&... args)
		{
			StateGroup sg;
			sg.mask = StateGroupMask::All;
			// 1. bind program & draw states (~= pipeline state)
			sg.drawStates = drawStates;
			// 1.1. bind framebuffer
			bind::framebuffer(fbo)(sg);
			// 2. bind dynamic args
			auto dummy = { (args(sg), 0)... };
			// 3. bind state group to pipeline
			bindStateGroup(sg);
			// 4. call render command
			drawCommand(sg);
		}

		
		inline void clear(Framebuffer& fb, const vec4& color) {
		  glClearNamedFramebufferfv(fb.object(), GL_COLOR, 0, &color[0]);
		}

		inline void clearTexture(Texture& tex, const vec4& color) {
		  glClearTexImage(tex.object(), 0, GL_RGBA, GL_FLOAT, &color[0]);
		}

		inline void clearDepth(Framebuffer& fb,
						float depth) {
		  glClearNamedFramebufferfv(fb.object(), GL_DEPTH, 0, &depth);
		}

		inline void clearDepthTexture(Texture& tex, float depth) {
		  glClearTexImage(tex.object(), 0, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
		}

		inline void clearInteger(Texture& tex, const ivec4& color) {
		  glClearTexImage(tex.object(), 0, GL_RGBA_INTEGER, GL_UNSIGNED_INT,
			  &color[0]);
		}

	}
}
