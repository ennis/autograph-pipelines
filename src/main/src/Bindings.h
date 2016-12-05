#pragma once 
#include <sol/state.hpp>

namespace ag 
{
	sol::table openLuaBindings(sol::this_state s);
}