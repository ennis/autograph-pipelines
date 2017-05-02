#include <autograph/Engine/Renderable.h>
#include <autograph/Engine/imgui.h>

namespace ag {
void RenderableComponents::showGUI(ID id) {
  if (auto p = get(id)) {
    if (ImGui::CollapsingHeader("Renderable")) {
      ImGui::TextDisabled("TODO...");
    }
  }
}
}
