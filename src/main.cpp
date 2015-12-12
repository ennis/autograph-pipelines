#include <iostream>
#include <format.h>
#include <glm/glm.hpp>

namespace ag
{
	template <typename TPixel>
	struct Surface
	{

	};

	struct GPUDomain
	{
		template <typename T> struct StreamDetail;
	};

	// GPUDomain::StreamDetail Surface<T> specialization
	template <typename TPixel> struct GPUDomain::StreamDetail<Surface<TPixel> >
	{
		StreamDetail()
		{
		}

		int thing;
		int width;
		int height;
	};

	template <typename T> struct GPUDomain::StreamDetail
	{
		StreamDetail()
		{
			std::clog << "Hello from StreamDetail" << std::endl;
		}

		T thing;
	};


	// T is signal type, D is execution domain
	template <
		typename T,
		typename D>
	struct Stream
	{
		int revision;
		typename D::template StreamDetail<T> detail;
	};

	// Lifts a lambda to operate on a signal
	// creates a node
	template <
		typename D,
		typename Fn, 
		typename ... TValues,
		typename R = std::result_of<Fn> > 
	auto MakeStream() -> Stream <R, D>
	{

	}
}

int main()
{
	using D = ag::GPUDomain;
	auto s1 = ag::Stream<int, D>();
	auto s2 = ag::Stream<ag::Surface<float>, D>();
}