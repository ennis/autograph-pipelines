#pragma once
#include "element.hpp"
#include "../observable.hpp"
#include "visual_state.hpp"

namespace ui 
{
	class button_behavior : public element 
	{
	public:
		button_behavior(element* parent) : element{parent}
		{}

		observable<> hover_enter;
		observable<> hover_exit;
		observable<> pressed;
		observable<> released;
		observable<> drag_enter;
		observable<> drag_exit;

		void update_input(int event)
		{
			// do hit-test
			// extract event params
			// update behaviors 
			// TODO mask unnecessary behaviors
		}

	private:
		glm::ivec2 drag_origin_;
		glm::ivec2 drag_current_;
		visual_state state_;
	};
}