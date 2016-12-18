#pragma once 
#include <sol/state.hpp>
#include "Effect.h"

namespace ag
{
	/////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////
	// Lua shader manager
	// Also manages caching?
	class ShaderManager
	{
	public:
		ShaderManager(sol::state& luaState);

		// load a shader resource in the context
		void loadShaderFile(const char* shaderId);

		template <typename... Args>
		std::unique_ptr<DrawPass> createDrawPass(const char* passName, Args&&... defines)
		{
			auto deftable = L.create_table_with(std::forward<Args>(defines)...);
			return createDrawPassInternal(passName, deftable);
		}

	private:
		std::unique_ptr<DrawPass> createDrawPassInternal(const char* passName, sol::table& table);
		sol::state& L;
	};
}