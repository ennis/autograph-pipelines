#pragma once
#include <tuple>
#include "bind.hpp"
#include "gl_pipeline.hpp"
#include "image.hpp"

////////////////////////// draw
// resources: buffer, images, etc.
/*template <typename DrawCommand, typename... ShaderResources>
void draw(gl_graphics_pipeline& pipeline,
		  framebuffer& target,
		  DrawCommand&& draw_command,
          ShaderResources&&... resources) 
{
	//
	gl_shader_resources gl_res;
	binder<framebuffer>::bind_gl(gl_res, target);
	for_each_in_tuple(std::forward_as_tuple<ShaderResources...>(std::forward<ShaderResources>(resources)...), [&gl_res](auto&& v) {
		using V = std::decay_t<decltype(v)>;
		typename binder<V>::bind_gl(gl_res, std::forward<V>(v));
	});
	// TODO: execute the draw command
}*/
