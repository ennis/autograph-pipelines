#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include <Stream.hpp>
#include <Utils.hpp>
#include <glm/glm.hpp>
#include <stdexcept>

namespace ag
{
	namespace graphics
	{
		// Forward decl
		struct DisplayNode;

		template <typename PixelType>
		struct ClearNode;

		// Surface usage hints
		enum class SurfaceUsageHints : unsigned
		{
			DefaultFramebuffer = 1 << 0,
		};
		template <> struct is_enum_flags<SurfaceUsageHints> : public std::true_type {};

		// Data access hints
		enum class DataAccessHints : unsigned
		{
			GPURead = 1 << 0,
			CPURead = 1 << 1
		};
		template <> struct is_enum_flags<DataAccessHints> : public std::true_type {};


		// Graphics context
		struct GraphicsContext
		{
			template <typename T> 
			struct StreamDetail;

			void setDisplayNode(std::shared_ptr<DisplayNode>&& display_)
			{
				if (display) {
					throw std::logic_error("setDisplayNode called twice");
				}
				display = std::move(display_);
			}

			std::shared_ptr<DisplayNode> display;
		};

		using D = GraphicsContext;
		
		// StreamDetail<Surface<PixelType> >
		template <typename TPixel> 
		struct GraphicsContext::StreamDetail<Surface<TPixel> >
		{
			SurfaceUsageHints hints;
		};

		// StreamDetail<T>
		template <typename T> 
		struct GraphicsContext::StreamDetail
		{
			DataAccessHints hints;
		};

		template <typename PixelType>
		struct ClearNode : StreamNode<Surface<PixelType>, D>
		{
			ClearNode(
				D& gc,
				Stream<glm::uvec2, D> size_, 
				Stream<glm::vec4, D> color_) : 
				size(std::move(size_)),
				color(std::move(color_))
			{
			}

			Stream<glm::uvec2, D> size;
			Stream<glm::vec4, D> color;

			Surface<PixelType> surface;
		};

		struct DisplayNode : public StreamNodeBase
		{
			DisplayNode(Stream<Surface<RGBA8>, D> rt_) : rt(std::move(rt_))
			{}

			Stream<Surface<RGBA8>, D> rt;
		};

		template <typename PixelType>
		auto clearRT(D& gc, Stream<glm::uvec2, D> size_, Stream<glm::vec4, D> color_) ->
			Stream<Surface<PixelType>, D>
		{
			auto ptr = std::make_shared<ClearNode<PixelType> >(gc, size_, color_);
			size_.ptr->successors.push_back(ptr.get());
			size_.ptr->detail.hints |= DataAccessHints::CPURead;
			color_.ptr->successors.push_back(ptr.get());
			color_.ptr->detail.hints |= DataAccessHints::CPURead;
			return Stream<Surface<PixelType>, D> { std::move(ptr) };
		}
		
		void displayRT(D& gc, Stream<Surface<glm::u8vec4>, D > rt)
		{
			auto ptr = std::make_shared<DisplayNode>(std::move(rt));
			ptr->rt.ptr->successors.push_back(ptr.get());
			ptr->rt.ptr->detail.hints |= SurfaceUsageHints::DefaultFramebuffer;
			gc.setDisplayNode(std::make_shared<DisplayNode>(std::move(rt)));
		}
	}
}

#endif // !GRAPHICS_HPP
