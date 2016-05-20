#pragma once
#include <tuple>
#include "bind.hpp"
#include "gl_pipeline.hpp"
#include "image.hpp"

////////////////////////// draw
// resources: buffer, images, etc.
template <typename DrawCommand, typename... ShaderResources>
void draw(gl_graphics_pipeline& pipeline,
		  DrawCommand&& draw_command,
          ShaderResources&&... resources) 
{
}
