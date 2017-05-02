#include <autograph/Engine/CVar.h>
#include <autograph/Engine/imgui.h>
#include <map>
#include <string>

namespace ag {
static std::map<std::string, CVarBase *> &getCVars();

////////////////////////////////////////////////////////////////
CVarBase::CVarBase(const char *id_) : id{ id_ }
{
	getCVars()[id_] = this;
}

////////////////////////////////////////////////////////////////
static std::map<std::string, CVarBase *> &getCVars() {
	static std::map<std::string, CVarBase *> cvars;
	return cvars;
}

void showCVarGui() {
  bool opened = true;
  static bool freezeFrame;
  ImGui::Begin("CVars", &opened, ImGuiWindowFlags_MenuBar);
  for (auto &p : getCVars()) {
    ImGui::Text(p.first.c_str());
  }
  ImGui::End();
}
}