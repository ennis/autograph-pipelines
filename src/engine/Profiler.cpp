#include <autograph/engine/ImGuiUtils.h>
#include <autograph/engine/Profiler.h>
#include <imgui.h>
#include <imgui_internal.h>

namespace ag {
namespace Profiler {
static bool isProfiling = false;
static Scope *prevScope = nullptr;
static Scope *curScope = nullptr;
static std::vector<std::unique_ptr<Scope>> prevProfile;
static std::vector<std::unique_ptr<Scope>> curProfile;
static ProfilingData data;

void beginFrame() {
  isProfiling = true;
  curProfile.clear();
  enterScope("<root>");
}

void endFrame() {
  if (!isProfiling)
    return;
  exitScope();
  isProfiling = false;
  prevProfile = std::move(curProfile);
  data.frameId = 0;
  data.rootScope = prevScope;
}

void enterScope(const char *scopeName) {
  if (!isProfiling)
    return;
  auto scope = std::make_unique<Scope>();
  scope->start = std::chrono::high_resolution_clock::now();
  scope->name = scopeName;
  scope->parent = curScope;
  if (curScope && !curScope->firstChild)
    curScope->firstChild = scope.get();
  else if (prevScope)
    prevScope->next = scope.get();
  curScope = scope.get();
  curProfile.push_back(std::move(scope));
}

void exitScope() {
  if (!isProfiling)
    return;
  curScope->end = std::chrono::high_resolution_clock::now();
  prevScope = curScope;
  curScope = const_cast<Scope*>(curScope->parent);
}

void event(const char *id) {
  if (!isProfiling)
    return;
  // TODO
}

const ProfilingData *getProfilingData() { return &data; }

static void scopeGui(const Scope *scope) {
  if (scope) {
    if (ImGui::TreeNode(scope->name.c_str())) {
      ImGui::Text("Duration %f", scope->duration());
      auto child = scope->firstChild;
      while (child) {
        scopeGui(child);
        child = child->next;
      }
      ImGui::TreePop();
    }
  }
}

void showGui() {
  ImGui::Begin("Profiler");
  scopeGui(data.rootScope);
  ImGui::End();
}
}
}