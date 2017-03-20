#include <autograph/engine/ImGuiUtils.h>
#include <autograph/engine/Profiler.h>
#include <autograph/engine/imgui.h>

namespace ag {
namespace Profiler {
static bool isProfiling = false;
static int prevScope = -1;
static int curScope = -1;
static std::vector<Scope> scopes;
static ProfileData lastData;
static ProfileData frozenData;

void beginFrame() {
  isProfiling = true;
  scopes.clear();
  prevScope = -1;
  curScope = -1;
  enterScope("<root>", true);
}

void endFrame() {
  if (!isProfiling)
    return;
  exitScope();
  isProfiling = false;
  lastData.frameId = 0;
  lastData.frameStartTime = scopes[0].start;
  lastData.frameEndTime = std::chrono::high_resolution_clock::now();
  lastData.scopes = std::move(scopes);
}

void enterScope(const char *scopeName, bool gpu) {
  if (!isProfiling)
    return;
  Scope s;
  int si = (int)scopes.size();
  s.start = std::chrono::high_resolution_clock::now();
  s.name = scopeName;
  s.parent = curScope;
  if (gpu) {
    s.gpuProfile = true;
    glGetInteger64v(GL_TIMESTAMP, &s.gpuClientTimestampRef);
    s.gpuTimestampStartQuery.asyncTimestamp();
  }
  if (curScope != -1 && scopes[curScope].firstChild == -1)
	  scopes[curScope].firstChild = si;
  else if (prevScope != -1)
	  scopes[prevScope].next = si;
  curScope = si;
  scopes.emplace_back(std::move(s));
}

void exitScope() {
  if (!isProfiling)
    return;
  auto& cs = scopes[curScope];
  cs.end = std::chrono::high_resolution_clock::now();
  if (cs.gpuProfile)
	  cs.gpuTimestampEndQuery.asyncTimestamp();
  prevScope = curScope;
  curScope = cs.parent;
}

void event(const char *id) {
  if (!isProfiling)
    return;
  // TODO
}

const ProfileData *getProfilingData() { return &lastData; }


static void profileGraphGui(const ProfileData &pfData, const Scope *scope) {
  float h = ImGui::GetItemsLineHeightWithSpacing();
  float w = ImGui::GetContentRegionAvailWidth();
  auto p = ImGui::GetCursorScreenPos();

  auto getTimelineX = [&](TimePoint time) {
    return (std::chrono::duration<float>{time - pfData.frameStartTime} /
            std::chrono::duration<float>{pfData.frameEndTime -
                                         pfData.frameStartTime}) *
           w;
  };

  auto drawList = ImGui::GetWindowDrawList();

  while (scope) {
    float startpos = getTimelineX(scope->start);
    float endpos = getTimelineX(scope->end);
    ImVec2 rmin = ImVec2{p.x + startpos, p.y};
    ImVec2 rmax = ImVec2{p.x + endpos, p.y + h};
    drawList->AddRectFilled(rmin, rmax, ImGui::GetColorU32(ImGuiCol_Button));
    drawList->PushClipRect(rmin, rmax);
    drawList->AddText(rmin, ImGui::GetColorU32(ImGuiCol_Text),
                      scope->name.c_str());
	drawList->PopClipRect();
	ImGui::SetCursorScreenPos(rmin);
	ImGui::Dummy(ImVec2{ endpos-startpos, h });
	if (ImGui::IsItemHovered()) {
		ImGui::BeginTooltip();
		ImGui::Text("%s", scope->name.c_str());
		ImGui::EndTooltip();
	}
    if (scope->firstChild)
      profileGraphGui(pfData, pfData.firstChild(scope));
    scope = pfData.next(scope);
  }
}

static void scopeGui(const ProfileData &pfData, const Scope *scope) {
  if (scope) {
    bool opened = false;
    if (scope->firstChild != -1)
      opened = ImGui::TreeNode(scope->name.c_str());
    else
      ImGui::BulletText("%s", scope->name.c_str());
    // CPU time
    ImGui::NextColumn();
    std::chrono::duration<double> cpu_fp_s = scope->duration();
    ImGui::Text("%f", cpu_fp_s);
    ImGui::NextColumn();
    // GPU time
    if (scope->gpuProfile) {
      std::chrono::duration<double> gpu_fp_s = scope->durationGpu();
      ImGui::Text("%f", gpu_fp_s);
    }
    ImGui::NextColumn();

    if (opened) {
		auto child = pfData.firstChild(scope);
      while (child) {
        scopeGui(pfData, child);
		child = pfData.next(child);
      }
      ImGui::TreePop();
    }
  }
}

void showGui() {
  bool opened = true;
  static bool freezeFrame;
  ImGui::Begin("Profiler", &opened, ImGuiWindowFlags_MenuBar);
  ImGui::BeginMenuBar();
  if (ImGui::Button("Freeze current frame")) {

  }
  // auto windowWidth = ImGui::GetWindowContentRegionWidth();
  // auto availHeight = ImGui::GetContentRegionAvail().y;
  ImGui::EndMenuBar();
  ImGui::Columns(3);
  ImGui::Text("Scope");
  ImGui::NextColumn();
  ImGui::Text("CPU time");
  ImGui::NextColumn();
  ImGui::Text("GPU time");
  ImGui::NextColumn();
  ImGui::Separator();
  scopeGui(lastData, lastData.root());
  ImGui::Columns(1);
  if (lastData.root()) {
    profileGraphGui(lastData, lastData.root());
  }
  ImGui::End();
}
}
}
