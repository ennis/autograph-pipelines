#pragma once
#include <autograph/support/Config.h>
#include <string>
#include <optional.hpp>

namespace ag 
{
	// while waiting for a standard implementation
	using std::experimental::optional;
	using std::experimental::nullopt;
	using std::experimental::nullopt_t;
}