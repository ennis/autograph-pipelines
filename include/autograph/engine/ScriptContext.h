#pragma once
#include <autograph/Engine/Exports.h>
#include <sol/state.hpp>

namespace ag {
class AG_ENGINE_API ScriptContext : public sol::state {
public:
  ScriptContext();
  ScriptContext(const char *initScript);

  sol::table scriptFile(const char *id);
  // this is a full path
  void addPackagePath(const char *path);
  void unloadModules();

private:
};
}