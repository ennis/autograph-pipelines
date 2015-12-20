#ifndef OPENGL_BACKEND_HPP
#define OPENGL_BACKEND_HPP

#include <Stream.hpp>
#include <Graphics.hpp>
#include <Utils.hpp>
#include <glm/glm.hpp>
#include <stdexcept>
#include <optional.hpp>

namespace ag
{
	namespace opengl
	{
		// Data access hints
		enum class DataAccessHints : unsigned
		{
			None = 0,
			GPURead = 1 << 0,
			CPURead = 1 << 1
		};

		// Surface usage hints
		enum class SurfaceUsageHints : unsigned
		{
			None = 0,
			DefaultFramebuffer = 1 << 0,
		};
	}
}

// enable the two previous enums to be used as bitflags
template <> struct is_enum_flags<ag::opengl::SurfaceUsageHints> : public std::true_type {};
template <> struct is_enum_flags<ag::opengl::DataAccessHints> : public std::true_type {};

namespace ag
{
	namespace opengl
	{
		// Graphics context (OpenGL)
		// Provides implementations of common and graphics nodes
		struct OpenGLBackend
		{
			struct NodeDetail
			{
				// nothing!
			};

			template <typename T> struct StreamDetail;

			// implementation templates of common nodes
			template <typename T> struct ConstNodeImpl;
			template <typename T> struct VarNodeImpl;

			// initialize(Node)
			//	  called on node creation
			//    initialize private data
			//
			// postInitialize(Node)
			//    called when the graph is finalized
			//    scan the graph, allocate and distribute resources accordingly
			//
			// execute(Node)
			//    executes the node
			//    all dependencies are guaranteed to be up-to-date

			template <typename T>
			void initialize(const T& node);

			//void initialize(const ag::graphics::DisplayNode<OpenGLBackend>& displayNode);

			// post-initialization methods
			template <typename T> void postInitialize(const VarNode<T, OpenGLBackend>& node);
			template <typename T> void postInitialize(const ConstNode<T, OpenGLBackend>& node);
			template <typename TPixel> void postInitialize(const StreamNode<Surface<TPixel>, OpenGLBackend>& node);
			void setDisplayNode(ag::graphics::DisplayNode<OpenGLBackend>& display_);

			template <typename T>
			void execute(T& node)
			{
				fmt::printf("Executing something?\n");
			}

			// load value into uniform buffer
			// load value to main memory
			// load surface into texture object
			// load surface to main memory

			std::experimental::optional<ag::graphics::DisplayNode<OpenGLBackend>*> display;
		};

		using D = OpenGLBackend;


		// specialization implementation of ConstNode<Surface<TPixel>>
		template <typename TPixel>
		struct OpenGLBackend::ConstNodeImpl<Surface<TPixel> >
		{
		};

		// template implementation of ConstNode<T> 
		template <typename T>
		struct OpenGLBackend::ConstNodeImpl
		{
			ConstNodeImpl(D& ctx, T init_value) : value(init_value)
			{
			}

			const T& get()
			{
				return value;
			}

			// value of the constant node
			T value;
		};

		// template implementation of VarNode<T> 
		template <typename T>
		struct OpenGLBackend::VarNodeImpl
		{
			VarNodeImpl(D& ctx, T init_value) : value(init_value)
			{}

			void set(T new_value)
			{
				value = new_value;
				// TODO update GPU buffer, if there is one
			}

			const T& get()
			{
				return value;
			}

			// value of the variable node
			T value;
		};
		
		
		// implementation detail for streams of Surface<PixelType>
		template <typename TPixel> 
		struct OpenGLBackend::StreamDetail<Surface<TPixel> >
		{
			SurfaceUsageHints hints = SurfaceUsageHints::None;
			int textureId = 0;
		};

		// StreamDetail<T>
		template <typename T> 
		struct OpenGLBackend::StreamDetail
		{
			DataAccessHints hints = DataAccessHints::None;
		};

		// 
		template <typename T>
		void OpenGLBackend::initialize(const T& node)
		{
			fmt::printf("Some node has been initialized\n");
		}

		template <>
		void OpenGLBackend::initialize(const ag::graphics::DisplayNode<OpenGLBackend>& displayNode)
		{
			displayNode.rt.detail.hints |= SurfaceUsageHints::DefaultFramebuffer;
			fmt::printf("A display node has been initialized\n");
		}

		// post-initialization methods
		template <typename T>
		void OpenGLBackend::postInitialize(const VarNode<T, OpenGLBackend>& node)
		{
			fmt::print("postInit: VarNode<T> @ %p\n");
			if (node.detail.hints & DataAccessHints::GPURead)
				fmt::printf("\tGPU-readable\n");
			if (node.detail.hints & DataAccessHints::CPURead)
				fmt::printf("\tCPU-readable\n");
		}

		template <typename T>
		void OpenGLBackend::postInitialize(const ConstNode<T, OpenGLBackend>& node)
		{
			fmt::print("postInit: ConstNode<T> @ %p\n");
			if (node.detail.hints & DataAccessHints::GPURead)
				fmt::printf("\tGPU-readable\n");
			if (node.detail.hints & DataAccessHints::CPURead)
				fmt::printf("\tCPU-readable\n");
		}

		template <typename TPixel>
		void OpenGLBackend::postInitialize(const StreamNode<Surface<TPixel>, OpenGLBackend>& node)
		{
			if (node.detail.hints & SurfaceUsageHints::DefaultFramebuffer)
				fmt::printf("postInit: StreamNode<Surface<TPixel> > @ %p is directed to the default framebuffer\n", &node);
		}

		void OpenGLBackend::setDisplayNode(ag::graphics::DisplayNode<OpenGLBackend>& display_)
		{
			if (display) {
				throw std::logic_error("setDisplayNode called twice");
			}
			display = &display_;
		}

	}
}

#endif // !OPENGL_BACKEND_HPP
