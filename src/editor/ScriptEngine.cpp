#include "ScriptEngine.h"
#include <autograph/support/ProjectRoot.h>
#include <cppformat/format.h>
#include <iostream>
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>


namespace ag {

static constexpr auto scriptEnginePath = "resources/scripts/ScriptEngine.dll";
// Paths are hardcoded for windows.
// On windows: the mono runtime should be installed alongside the application
// On linux: use the package manager
// TODO at least provide this as a configuration option in CMake or in the
// command line
static constexpr auto monoAssemblyDir = "C:\\Program Files\\Mono\\lib";
static constexpr auto monoConfigDir = "C:\\Program Files\\Mono\\etc";

ScriptEngine::ScriptEngine() {
  fmt::print(std::clog, "Initializing Mono JIT engine...\n");
  mono_set_dirs(monoAssemblyDir, monoConfigDir);
  domain_ = mono_jit_init("editor");
  fmt::print(std::clog, "Mono JIT engine initialized\n");
  // Before loading script assemblies, we copy them into a temporary directory
  // This is to allow a build system to overwrite the original assemblies,
  // since any assembly loaded by mono is locked.
  // We create the temporary directory here.
  // Load the ScriptEngine assembly
  // TODO This is hardcoded and it should not be
  assembly_ = loadAssembly(getProjectRootDirectory() + "/" + scriptEnginePath);
  image_ = mono_assembly_get_image(assembly_);
  callInitializeMethod();
}

void ScriptEngine::onRender(void * skSurface)
{
	void *params[] = { &skSurface };
	mono_runtime_invoke(render_, nullptr, params, nullptr);
}

MonoAssembly *
ScriptEngine::loadAssembly(const std::experimental::filesystem::path &path) {
  namespace fs = std::experimental::filesystem;
  auto pathstr = path.string();
  auto assembly = mono_domain_assembly_open(domain_, pathstr.c_str());
  if (!assembly) {
    fmt::print(std::cerr, "Error loading assembly: {}\n", pathstr);
    throw std::runtime_error("Error loading assembly");
  }
  fmt::print(std::clog, "Loaded assembly {}\n", pathstr.c_str());
  return assembly;
}

MonoMethod * ScriptEngine::findMethod(const char * name)
{
	auto methodDesc = mono_method_desc_new(name, false);
	auto method =
		mono_method_desc_search_in_image(methodDesc, image_);
	return method;
}

// Initialize managed ScriptEngine
void ScriptEngine::callInitializeMethod() {
  const char *initMethodName = "Autograph.ScriptEngine:Initialize(string)";
  auto initializeMethod = findMethod(initMethodName);
  if (!initializeMethod) {
    fmt::print(std::cerr,
               "Could not initialize script assembly: method {} not found\n",
               initMethodName);
    return;
  }

  auto rootPathStr = getProjectRootDirectory();
  auto rootPathStrManaged = mono_string_new(domain_, rootPathStr.c_str());
  MonoObject *exc;
  void *params[] = {rootPathStrManaged};
  mono_runtime_invoke(initializeMethod, nullptr, params, &exc);
  if (exc) {
    fmt::print(std::cerr, "Exception raised while initializing assembly\n");
  }

  render_ = findMethod("Autograph.ScriptEngine:RenderInternal(intptr)");
  if (!render_) {
	  fmt::print(std::cerr,
		  "Could not find Autograph.ScriptEngine:RenderInternal(int)\n");
  }
}
}