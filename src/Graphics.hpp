// Graphics nodes
#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include <Surface.hpp>
#include <Stream.hpp>
#include <optional.hpp>

namespace ag {
namespace graphics {

	template <
		typename PixelType, 
		typename D
	>
	struct ClearNode : StreamNode<Surface<PixelType>, D>
	{
		ClearNode(
			D& backend,
			StreamNode<glm::uvec2, D>& size_,
			StreamNode<glm::vec4, D>& color_) :
		size(size_),
		color(color_)
		{
			backend.initialize(*this);
		}

		void execute(D& backend) override
		{
			backend.execute(*this);
		}

		StreamNode<glm::uvec2, D>& size;
		StreamNode<glm::vec4, D>& color;
		Surface<PixelType> surface;
	};

	template <
		typename D
	>
	struct DisplayNode : public StreamNodeBase<D>
	{
		DisplayNode(
			D& backend,
			StreamNode<Surface<RGBA8>, D> &rt_) :
			rt(rt_)
		{
			backend.initialize(*this);
		}

		void execute(D& backend) override
		{
			// if rt is not default framebuffer
			//		ctx.copySurface(rt, screen)
			// ctx.swapBuffers()
			backend.execute(*this);
		}

		StreamNode<Surface<RGBA8>, D>& rt;
	};
}
}

#endif // !GRAPHICS_HPP
