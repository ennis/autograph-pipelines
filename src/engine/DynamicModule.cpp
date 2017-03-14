#include <Windows.h>
#include <autograph/engine/DynamicModule.h>
#include <autograph/support/Debug.h>
#include <vector>

namespace ag {
#ifdef AG_WINDOWS
using ModuleHandle = HMODULE;
#else
using ModuleHandle = void *;
#endif

using ModuleInitFunc = void (*)();
using ModuleExitFunc = void (*)();

static const char kModuleInitFunctionName[] = "moduleInit";
static const char kModuleExitFunctionName[] = "moduleExit";

struct Module
{
	std::string name;
	ModuleHandle hmod;
};

struct DynamicModules {
  std::vector<ModuleHandle> loadedModules;
};

static DynamicModules &getDynamicModules() {
  static DynamicModules sDynamicModules;
  return sDynamicModules;
}

void loadDynamicModule(const char *name) {
  std::string path = std::string{name} + ".dll";
#ifdef AG_WINDOWS
  HMODULE mod = LoadLibraryA(path.c_str());
  if (!mod) {
    errorMessage("Error loading dynamic module {}", path.c_str());
    return;
  }
  // Find entry point
  auto proc = (ModuleInitFunc)GetProcAddress(mod, kModuleInitFunctionName);
  if (!proc) {
    errorMessage("Module entry point ({}) not found in dynamic module {}",
                 kModuleInitFunctionName, path.c_str());
    return;
  }
  // Call entry point
  proc();
#endif
}

void unloadDynamicModule(const char *path) {}

void unloadDynamicModules() {}

void *createClassInstance(const char *className) { return nullptr; }
}