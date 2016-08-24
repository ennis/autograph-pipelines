#pragma once
#include "entity.hpp"
#include "coroutine.hpp"

class coroutine_script : public script_base
{
public:
	template <typename F, typename... Args>
	coroutine_script(F&& f, Args&&... args) : c{ std::forward<F>(f), std::forward<Args>(args)... }
	{}

	virtual ~coroutine_script()
	{
		// somehow stop the coroutine?
	}

private:
	coroutine c;
};
