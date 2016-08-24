#pragma once
#include "../entity.hpp"
#include <reflect.hpp>

namespace ui
{
	struct REFLECT selectable : public behaviour<selectable> {
		virtual ~selectable() {}

		virtual bool on_pointer_down() { return true; }

		// selectable *select_left = nullptr;
		// selectable *select_right = nullptr;
		// selectable *select_down = nullptr;
		// selectable *select_up = nullptr;
	};
}