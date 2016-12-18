#include "ShaderManager.h"
#include <autograph/support/Debug.h>

namespace ag
{
	ShaderManager::ShaderManager(sol::state& luaState) : L{ luaState }
	{}

	void ShaderManager::loadShaderFile(const char* shaderId)
	{
		AG_DEBUG("ShaderManager::loadShaderFile: {}", shaderId);
		L.script(std::string{ "require '" } +shaderId + "'");
	}

	std::unique_ptr<DrawPass> ShaderManager::createDrawPassInternal(const char* passName, sol::table& table)
	{
		AG_DEBUG("ShaderManager::createDrawPassInternal: {}", passName);
		auto config = L["autograph"]["createShaderFromTemplate"](passName, table);
		DrawPassBuilder builder;
		builder.loadFromTable(config);
		return builder.makeDrawPass();
	}
}