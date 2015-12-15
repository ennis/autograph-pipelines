#include <iostream>
#include <format.h>
#include <glm/glm.hpp>

#include <PixelType.hpp>
#include <Surface.hpp>
#include <Stream.hpp>
#include <Graphics.hpp>
#include <Utils.hpp>

//#include <optional.hpp>
#include <utility>


/*namespace ag
{
	using namespace gl;
	
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
}*/

int main()
{
	glbinding::Binding::initialize();

	using D = ag::graphics::GraphicsContext;
	D ctx;

	auto color = ag::constant<glm::vec4>(ctx, {0.0f, 0.0f, 1.0f, 1.0f});
	auto size = ag::constant<glm::uvec2>(ctx, { 1280, 720 });
	auto rt = ag::graphics::clearRT<ag::RGBA8>(ctx, size, color);
	ag::graphics::displayRT(ctx, rt);

	auto color2 = ag::apply(
		[](glm::vec4 c) -> glm::vec4 {
			return c + glm::vec4{1.0f, 0.0f, 0.0f, 0.0f};
		}, color);


	return 0;
}
