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

struct Module {
  //////////////////////////////////////////
  // Default ctor
  Module();
  // Constructor
  Module(std::experimental::filesystem::path path_);
  // Reload shared library and call entry point
  void reload();

  //////////////////////////////////////////
  std::experimental::filesystem::path path;
  ModuleHandle moduleHandle = nullptr;
  std::experimental::filesystem::file_time_type lastWriteTime;
};

namespace fs = std::experimental::filesystem;
using ModuleInitFunc = void (*)();
using ModuleExitFunc = void (*)();

static constexpr const char kModuleInitFunctionName[] = "moduleInit";
static constexpr const char kModuleExitFunctionName[] = "moduleExit";

// Default ctor
Module::Module() {}

// Constructor
Module::Module(fs::path path_) : path{std::move(path_)} { reload(); }

// Reload shared library and call entry point
void Module::reload() {
  std::error_code errc;
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
  fs::copy_file(path, tmpPath, fs::copy_options::overwrite_existing, errc);
  if (errc) {
    errorMessage("Error copying file (origPath={})", path.string());
    errorMessage("\t{}", errc.message());
    moduleHandle = nullptr;
    return;
  }
  // load the library
  HMODULE hmod = LoadLibraryA(tmpPath.string().c_str());
  if (!hmod) {
    errorMessage("Error loading dynamic module (tmpPath={}, origPath={})",
                 tmpPath.string(), path.string());
    moduleHandle = nullptr;
    return;
  }
  // Find entry point
  auto proc = (ModuleInitFunc)GetProcAddress(hmod, kModuleInitFunctionName);
  if (!proc) {
    errorMessage("Module entry point ({}) not found", kModuleInitFunctionName);
    FreeLibrary(hmod);
    // Module is in 'Unloaded' state
    moduleHandle = nullptr;
    return;
  }
  //
  moduleHandle = hmod;
  lastWriteTime = fs::last_write_time(path, errc);
  // if (errc)
  // AG_DEBUG("Error {}", errc);
  // Call entry point
  proc();
}

ReloadablePluginProxy::ReloadablePluginProxy(
    const char *className_, std::type_index interfaceTypeIndex_)
    : module{nullptr}, className{className_}, interfaceTypeIndex{
                                                  interfaceTypeIndex_} {}

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

  // Create a class instance of the type specified by the factory
  // Effectively creates a plugin instance and returns a proxy object for
  // hot-reloading
  // TODO: should probably return a shared_ptr
  ReloadablePluginProxy *
  createReloadablePluginProxy(const char *className,
                              std::type_index interfaceTypeIndex,
                              bool createInstance = true) {
    auto proxyUnique =
        std::make_unique<ReloadablePluginProxy>(className, interfaceTypeIndex);
    auto proxy = proxyUnique.get();
    reloadablePluginProxies.push_back(std::move(proxyUnique));
    // Create the instance (may fail)
    if (createInstance)
      proxy->create();
    return proxy;
  }

  // Reload the specified module
  void reloadModule(Module &m) {
    // delete all class factories provided by this module
    for (auto it = classFactories.begin(); it != classFactories.end(); ++it) {
      if ((*it).second && (*it).second->module == &m) {
        classFactories.erase(it);
      }
    }
    // go through all plugin instances that use code from this module and delete
    // them
    for (auto &pluginProxy : reloadablePluginProxies) {
      if (pluginProxy->module == &m) {
        // pluginProxy->module = nullptr;
        pluginProxy->plugin.reset();
      }
    }

    // Reload the shared library: this gives the opportunity to the plugin to
    // register its plugin factories
    currentLoadingModule = &m;
    m.reload();
    // Now that the factories are registered, try to restore the plugin
    // instances
    for (auto &pluginProxy : reloadablePluginProxies) {
      AG_DEBUG("Reloading plugin instance {}.", pluginProxy->className);
      if (!pluginProxy->module || pluginProxy->module == &m) {
        pluginProxy->create();
        // TODO: deserialize
      }
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
    // add factory to module instance?
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
        reloadModule(mod);
      }
    }
  }

private:
  Module *currentLoadingModule = nullptr;
  std::unordered_map<std::string, Module> loadedModules;
  std::vector<std::unique_ptr<ReloadablePluginProxy>> reloadablePluginProxies;
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
createReloadablePluginProxy(const char *name, std::type_index interfaceTypeID) {
  return getPluginManager().createReloadablePluginProxy(name, interfaceTypeID);
}

void loadPluginModule(const char *name) { getPluginManager().loadModule(name); }

void unloadPluginModule(const char *path) {
  // TODO
}

bool ReloadablePluginProxy::create() {
  auto factory = getPluginManager().getClassFactory(className.c_str());
  if (!factory) {
    // Type not found in registered factories
    errorMessage("Plugin class {} is unavailable.", className.c_str());
    return false;
  }
  if (!factory->implementsInterface(interfaceTypeIndex)) {
    // the type does not implement the desired interface
    errorMessage("Plugin class {} does not implement the desired interface",
                 className.c_str());
    return false;
  }
  // Factory found and the type implements the desired interface
  // Create the plugin instance
  AG_DEBUG("Creating instance of class {}.", className.c_str());
  plugin = factory->createClassInstance();
  module = factory->module;
  return true;
}

void watchForPluginModuleChanges() { getPluginManager().watchForChanges(); }

void unloadAllPluginModules() {
  // TODO
}
} // namespace ag