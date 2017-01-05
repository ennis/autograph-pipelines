#pragma once 
#include <sol/state.hpp>

namespace ag 
{
	namespace detail {
		sol::table openLuaBindings(sol::this_state s);
	}
}