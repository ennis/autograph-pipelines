#include <iostream>
#include <format.h>
#include <glm/glm.hpp>

#include <PixelType.hpp>
#include <Surface.hpp>
#include <Stream.hpp>
#include <backend/opengl/Backend.hpp>
#include <backend/opengl/OpenGL.hpp>
#include <Utils.hpp>
#include <Pipeline.hpp>

#include <optional.hpp>
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

	using D = ag::opengl::OpenGLBackend;
	D backend;

	ag::Pipeline<D> pp(backend);

	auto& frameTrigger = ag::variable(pp, 0.0);
	auto& color = ag::constant<glm::vec4>(pp, {0.0f, 0.0f, 1.0f, 1.0f});
	auto& size = ag::constant<glm::uvec2>(pp, { 1280, 720 });
	auto& rt = ag::clearRT<ag::RGBA8>(pp, size, color);
	ag::displayRT<D>(pp, rt);

	auto color2 = ag::apply(
		pp, 
		[](glm::vec4 c) -> glm::vec4 {
			return c + glm::vec4{1.0f, 0.0f, 0.0f, 0.0f};
		}, color);

	pp.schedule(color);


	return 0;
}
