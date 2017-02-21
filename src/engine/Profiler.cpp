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
  enterScope("<root>", true);
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

void enterScope(const char *scopeName, bool gpu) {
  if (!isProfiling)
    return;
  auto scope = std::make_unique<Scope>();
  scope->start = std::chrono::high_resolution_clock::now();
  scope->name = scopeName;
  scope->parent = curScope;
  if (gpu) {
    scope->gpuProfile = true;
    glGetInteger64v(GL_TIMESTAMP, &scope->gpuClientTimestampStart);
    scope->gpuTimestampStartQuery.asyncTimestamp();
  }
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
  if (curScope->gpuProfile)
    curScope->gpuTimestampEndQuery.asyncTimestamp();
  prevScope = curScope;
  curScope = const_cast<Scope *>(curScope->parent);
}

void event(const char *id) {
  if (!isProfiling)
    return;
  // TODO
}

const ProfilingData *getProfilingData() { return &data; }

static void scopeGui(const Scope *scope) {
  if (scope) {
    bool opened = false;
    if (scope->firstChild)
     opened = ImGui::TreeNode(scope->name.c_str());
    else
     ImGui::BulletText("%s", scope->name.c_str());
    // CPU time
    ImGui::NextColumn();
    ImGui::Text("%f", scope->duration());
    ImGui::NextColumn();
    // GPU time
    if (scope->gpuProfile)
      ImGui::Text("%f", scope->durationGpu());
    ImGui::NextColumn();

    if (opened) {
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
    ImGui::ShowTestWindow();
  bool opened = true;
  ImGui::Begin("Profiler", &opened, ImGuiWindowFlags_MenuBar);
  ImGui::BeginMenuBar();
  if (ImGui::Button("Capture current frame")) {
    // TODO
  }
  ImGui::EndMenuBar();
  ImGui::Columns(3);
  ImGui::Text("Scope");
  ImGui::NextColumn();
  ImGui::Text("CPU time");
  ImGui::NextColumn();
  ImGui::Text("GPU time");
  ImGui::NextColumn();
  ImGui::Separator();
  scopeGui(data.rootScope);
  ImGui::Columns(1);
  ImGui::End();
}
}
}
