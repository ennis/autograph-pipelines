#pragma once
#include "../entity.hpp"

namespace ui
{
	enum class state {
		Released,
		Hovered,
		Pressed
	};

	struct state_component : public component<state_component> {
		state state;
	};
}