#include <autograph/Engine/Light.h>
#include <autograph/Engine/imgui.h>

namespace ag {
void LightComponents::showGUI(ID id) {
  if (auto p = get(id)) {
    if (ImGui::CollapsingHeader("Light")) {
      ImGui::TextDisabled("TODO...");
    }
  }
}
}
