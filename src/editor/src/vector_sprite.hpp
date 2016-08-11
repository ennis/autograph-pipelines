#pragma once
#include <glm/glm.hpp>
#include <functional>
#include <reflect.hpp>
#include "entity.hpp"

struct NVGcontext;

// vector graphics component
struct vector_sprite : public component<vector_sprite>
{
	using callback_t = std::function<void(NVGcontext *, glm::vec2)>;
	callback_t commands;
};
