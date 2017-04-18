#include <autograph/engine/Light.h>
#include <autograph/engine/imgui.h>

namespace ag {
void LightComponents::showGUI(ID id) {
  if (auto p = get(id)) {
    if (ImGui::CollapsingHeader("Light")) {
      ImGui::TextDisabled("TODO...");
    }
  }
}
}
