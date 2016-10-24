#pragma once
#include <experimental/filesystem>
#include <mono/metadata/metadata.h>

namespace ag {
//
// Mono JIT engine and assembly manager
class ScriptEngine {
public:
  ScriptEngine();

  void onRender(void* skSurface);

private:
  MonoAssembly *loadAssembly(const std::experimental::filesystem::path &path);
  MonoMethod *findMethod(const char* id);

  // Initialize managed ScriptEngine
  void callInitializeMethod();
  MonoDomain *domain_{nullptr};
  MonoAssembly *assembly_{nullptr};
  MonoImage *image_{nullptr};
  MonoMethod *render_{ nullptr };
  std::experimental::filesystem::path tempAssemblyDir;
};
}