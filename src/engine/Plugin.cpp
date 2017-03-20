#include <Windows.h>
#include <autograph/engine/Plugin.h>
#include <autograph/support/Debug.h>
#include <autograph/support/ProjectRoot.h>
#include <filesystem>
#include <unordered_map>
#include <vector>

namespace ag {
#ifdef AG_WINDOWS
using ModuleHandle = HMODULE;
#else
using ModuleHandle = void *;
#endif

namespace fs = std::experimental::filesystem;
using ModuleInitFunc = void (*)();
using ModuleExitFunc = void (*)();

static constexpr const char kModuleInitFunctionName[] = "moduleInit";
static constexpr const char kModuleExitFunctionName[] = "moduleExit";

struct Module {
  fs::path path;
  ModuleHandle moduleHandle;
  std::experimental::filesystem::file_time_type lastWriteTime;
  // List of reload proxies for plugins provided by this module
  std::vector<std::unique_ptr<ReloadablePluginProxy>> reloadablePluginProxies;

  //
  ReloadablePluginProxy *createClassInstance(PluginClassFactory &factory) {
    auto proxy = std::make_unique<ReloadablePluginProxy>();
    proxy->className = factory.className;
    proxy->plugin = factory.createClassInstance();
    auto ptr = proxy.get();
    reloadablePluginProxies.push_back(std::move(proxy));
    return ptr;
  }

  // Reload shared library and call entry point
  void reload() {
    AG_DEBUG("reload moduleHandle={}, path={}", (intptr_t)moduleHandle,
             path.string());
    // If there is a module loaded, unload it
    if (moduleHandle) {
      FreeLibrary(moduleHandle);
    }
    // copy DLL to a temporary directory and load it from there
    // to avoid locking the original file and allow someone to replace it
    auto tmp = fs::temp_directory_path();
    auto tmpPath = tmp / path.filename();
    fs::copy_file(path, tmpPath, fs::copy_options::overwrite_existing);
    // load the library
    HMODULE hmod = LoadLibraryA(tmpPath.string().c_str());
    if (!hmod) {
      errorMessage("Error loading dynamic module (tmpPath={}, origPath={})",
                   tmpPath.string(), path.string());
      return;
    }
    // Find entry point
    auto proc = (ModuleInitFunc)GetProcAddress(hmod, kModuleInitFunctionName);
    if (!proc) {
      errorMessage("Module entry point ({}) not found",
                   kModuleInitFunctionName);
      return;
    }
    //
    moduleHandle = hmod;
    std::error_code errc;
    lastWriteTime = fs::last_write_time(path, errc);
    if (errc)
      AG_DEBUG("Error {}", errc);
    // Call entry point
    proc();
  }
};

class PluginManager {
public:
  // Load a module
  void loadModule(const char *name) {
    // get the module called 'name', or create a new one if necessary
    auto &m = loadedModules[name];
    // build DLL path
    auto projRoot = getProjectRootDirectory();
    auto fullPath = fs::path{projRoot} / "build/RelWithDebInfo" /
                    name; // XXX this is hardcoded but we don't want to load
                          // plugins from anywhere
    fullPath.replace_extension(".dll");
    m.path = fullPath;
    reloadModule(m);
  }

  // Reload the specified module
  void reloadModule(Module &m) {
    // delete all class factories provided by this module
    for (auto it = classFactories.begin(); it != classFactories.end(); ++it) {
      if ((*it).second->module == &m) {
        classFactories.erase(it);
      }
    }
    // go through all plugin instances that use code from this module and delete
    // them
    for (auto &pluginInstance : m.reloadablePluginProxies) {
      pluginInstance->plugin.reset();
    }

    // Reload the shared library: this gives the opportunity to the plugin to
    // register its plugin factories
    currentLoadingModule = &m;
    m.reload();
    // Now that the factories are registered, try to restore the plugin
    // instances
    for (auto &pluginInstance : m.reloadablePluginProxies) {
      auto factory = getClassFactory(pluginInstance->className.c_str());
      if (!factory) {
        errorMessage("Class {} is not available.", pluginInstance->className);
        continue;
      }
      pluginInstance->plugin = factory->createClassInstance();
      // TODO: deserialize
    }
  }

  // Find a class factory by name
  PluginClassFactory *getClassFactory(const char *name) {
    return classFactories[name].get();
  }

  // Add a class factory
  void addClassFactory(const char *name,
                       std::unique_ptr<PluginClassFactory> factory) {
    factory->module = currentLoadingModule; // Hack?
    classFactories[name] = std::move(factory);
  }

  // Effectively creates a plugin instance and returns a proxy object for
  // hot-reloading
  ReloadablePluginProxy *
  createReloadablePluginProxy(PluginClassFactory &factory) {
	  AG_DEBUG("Creating instance of class {}", factory.className);
    return factory.module->createClassInstance(factory);
  }

  //
  void watchForChanges() {
    for (auto &pair : loadedModules) {
      auto &mod = pair.second;
      std::error_code errc;
      auto lastWriteTime = fs::last_write_time(mod.path, errc);
      if (errc) {
        // AG_DEBUG("Error ({}) getting last write time for module {}",
        //         errc.message(), mod.path.string());
      } else if (mod.lastWriteTime < lastWriteTime) {
        mod.lastWriteTime = lastWriteTime;
        AG_DEBUG("Module change detected: {}", mod.path.string());
        mod.reload();
      }
    }
  }

private:
  Module *currentLoadingModule = nullptr;
  std::unordered_map<std::string, Module> loadedModules;
  std::unordered_map<std::string, std::unique_ptr<PluginClassFactory>>
      classFactories;
};

static auto &getPluginManager() {
  static PluginManager pm;
  return pm;
}

void registerPluginClassFactory(const char *name,
                                std::unique_ptr<PluginClassFactory> factory) {
  getPluginManager().addClassFactory(name, std::move(factory));
  AG_DEBUG("Registered class: {}", name);
}

PluginClassFactory *getPluginClassFactory(const char *name) {
  return getPluginManager().getClassFactory(name);
}

ReloadablePluginProxy *
createReloadablePluginProxy(PluginClassFactory &factory) {
  return getPluginManager().createReloadablePluginProxy(factory);
}

void loadPluginModule(const char *name) { getPluginManager().loadModule(name); }

void unloadPluginModule(const char *path) {
  // TODO
}

void watchForPluginModuleChanges() { getPluginManager().watchForChanges(); }

void unloadAllPluginModules() {
  // TODO
}
}