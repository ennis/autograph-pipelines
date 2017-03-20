#pragma once
#include <autograph/engine/Config.h>
#include <memory>
#include <string>
#include <typeindex>

namespace ag {

//////////////////////////////////////////////
// Plugin module API
// AG_ENGINE_API int getPluginModuleCount();
// AG_ENGINE_API int getPluginModule(int id);
AG_ENGINE_API void loadPluginModule(const char *path);
AG_ENGINE_API void unloadPluginModule(const char *path);
AG_ENGINE_API void watchForPluginModuleChanges();
AG_ENGINE_API void unloadAllPluginModules();

//////////////////////////////////////////////
// Base class for all plugin interfaces
class Plugin {
public:
  virtual void getName() {}

private:
};

class Extension : public Plugin {
public:
  virtual void onGUI() {}
  virtual void onMainMenuGUI() {}
};

//////////////////////////////////////////////
// Plugin class registration API
struct ReloadablePluginProxy {
  std::string className;
  std::shared_ptr<Plugin> plugin;
};

struct Module;

struct PluginClassFactory {
  Module *module = nullptr;
  std::string className;

  virtual std::shared_ptr<Plugin> createClassInstance() const = 0;
  virtual bool implementsInterface(std::type_index iface_ti) const = 0;
};

//////////////////////////////////////////////
// Smart pointer for hot-reloadable plugin interfaces
template <typename T> class PluginPtr {
public:
  PluginPtr() : control_{nullptr} {}

  PluginPtr(ReloadablePluginProxy *control) : control_{control} {}

  auto operator-> () { return get(); }
  const auto operator-> () const { return get(); }
  T *get() {
    if (!control_)
      return nullptr;
    return static_cast<T *>(control_->plugin.get());
  }
  const T *get() const {
    if (!control_)
      return nullptr;
    return static_cast<const T *>(control_->plugin.get());
  }

  explicit operator bool() const { return get(); }

private:
  ReloadablePluginProxy *control_;
};

//////////////////////////////////////////////
AG_ENGINE_API void
registerPluginClassFactory(const char *name,
                           std::unique_ptr<PluginClassFactory> reg);
AG_ENGINE_API PluginClassFactory *getPluginClassFactory(const char *name);
AG_ENGINE_API ReloadablePluginProxy *
createReloadablePluginProxy(PluginClassFactory &factory);

template <typename T> PluginPtr<T> createClassInstance(const char *className) {
  auto factory = getPluginClassFactory(className);
  if (!factory || !factory->implementsInterface(typeid(T))) {
    return nullptr;
  }
  return PluginPtr<T>{createReloadablePluginProxy(*factory)};
}

template <typename T, typename... Interfaces>
void registerClass(const char *name) {
  struct Factory : public PluginClassFactory {
    virtual std::shared_ptr<Plugin> createClassInstance() const override {
      return std::make_shared<T>();
    }

    virtual bool implementsInterface(std::type_index iface_ti) const override {
      bool ok = false;
      auto check_iface = [&](std::type_index ti2) mutable {
        ok |= (iface_ti == ti2);
      };
      auto dummy = {(check_iface(typeid(Interfaces)), 0)...};
      return ok;
    }
  };
  auto factory = std::make_unique<Factory>();
  factory->module = nullptr; // TODO get current module?
  factory->className = name;
  registerPluginClassFactory(name, std::move(factory));
}
}
