#include "Bindings.h"
#include <autograph/Engine/ResourceManager.h>
#include <autograph/Engine/ScriptContext.h>
#include <vector>

std::vector<std::string> globalPackagePaths;

namespace ag {

ScriptContext::ScriptContext() {
  // open required libraries
  open_libraries(sol::lib::base, sol::lib::package, sol::lib::debug,
                 sol::lib::string, sol::lib::io, sol::lib::math,
                 sol::lib::coroutine, sol::lib::os, sol::lib::table,
				sol::lib::bit32);

  // register application resource paths
  int numResourceDirs = ResourceManager::getResourceDirectoriesCount();
  for (int i = 0; i < numResourceDirs; ++i) {
	auto resPath = ResourceManager::getResourceDirectory(i);
    addPackagePath((resPath + "?.lua").c_str());
    addPackagePath((resPath + "scripts/?.lua").c_str());
  }

  // load bindings and helper module
  require("autograph_bindings", sol::c_call<decltype(&detail::openLuaBindings),
                                            &detail::openLuaBindings>);
  script("require 'autograph'");
}

void ScriptContext::addPackagePath(const char *path) {
  std::string package_path = (*this)["package"]["path"];
  (*this)["package"]["path"] =
      package_path + (!package_path.empty() ? ";" : "") + path;
}

void ScriptContext::unloadModules() {
  script(R"(
		for k,v in pairs(package.loaded) do
			print("Unloading package " .. k)
			package.loaded[k] = nil
		end
		)");
}

ScriptContext::ScriptContext(const char *initScript) : ScriptContext{} {
  script(std::string{"require '"} + initScript + "'");
}

sol::table ScriptContext::scriptFile(const char *id) {
  auto path = ResourceManager::getFilesystemPath(id);
  return script_file(path);
}
}
