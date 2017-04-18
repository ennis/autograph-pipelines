#pragma once
#include <string>
#include <editor/ui/Widget.hpp>
#include <editor/ui/DefaultStyle.hpp>

namespace ui {
enum class REFLECT FontWeight { Thin, Regular, Bold };

enum class REFLECT TextAlignment {
	Left M_FRIENDLY_NAME("Left"), 
	Right M_FRIENDLY_NAME("Right"),
	Center M_FRIENDLY_NAME("Center"),
	Top M_FRIENDLY_NAME("Top"),
	Bottom M_FRIENDLY_NAME("Bottom"),
	Middle M_FRIENDLY_NAME("Middle"),
	Baseline M_FRIENDLY_NAME("Baseline")
};

struct Text : public Widget {
	Text() = default;
	Text(std::string text_, float textSize_, glm::vec4 color_,
              bool auto_fit_ = false)
      : text{text_}, textSize{ textSize_ }, color{color_}, autoFit{auto_fit_} {
  }

  void render(HUDRenderer& renderer, glm::vec2 size) override;

  std::string text M_FRIENDLY_NAME("Text");
  float textSize M_FRIENDLY_NAME("Text size");
  glm::vec4 color M_FRIENDLY_NAME("Color");
  bool autoFit M_FRIENDLY_NAME("Auto fit") { false };
  TextAlignment alignment M_FRIENDLY_NAME("Alignment") { TextAlignment::Left };
};

}