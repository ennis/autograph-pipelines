#pragma once
#include "visual.hpp"
#include <nanovg.h>
#include <string>

namespace ui {
enum class REFLECT font_weight { thin, regular, bold };

enum class REFLECT text_alignment {
	left META_FRIENDLY_NAME("Left"), 
	right META_FRIENDLY_NAME("Right"),
	center META_FRIENDLY_NAME("Center"),
	top META_FRIENDLY_NAME("Top"),
	bottom META_FRIENDLY_NAME("Bottom"),
	middle META_FRIENDLY_NAME("Middle"),
	baseline META_FRIENDLY_NAME("Baseline")
};

struct REFLECT text_visual : public visual {
  text_visual() = default;
  text_visual(std::string text_, float font_size_, glm::vec4 color_,
              bool auto_fit_ = false)
      : text{text_}, font_size{font_size_}, color{color_}, auto_fit{auto_fit_} {
  }

  void render(NVGcontext *nvg, glm::vec2 size) override {
    nvgTextAlign(nvg, NVG_ALIGN_CENTER | NVG_ALIGN_TOP);
    if (auto_fit)
      nvgFontSize(nvg, size.y * 0.9f);
    else
      nvgFontSize(nvg, font_size);
    nvgFillColor(nvg, nvgRGBAf(color.r, color.g, color.b, color.a));
    nvgTextBox(nvg, 0.0f, 0.0f, size.x, text.c_str(), nullptr);
  }

  std::string text META_FRIENDLY_NAME("Text");
  float font_size META_FRIENDLY_NAME("Font size");
  glm::vec4 color META_FRIENDLY_NAME("Color");
  bool auto_fit META_FRIENDLY_NAME("Auto fit") { false };
  text_alignment alignment META_FRIENDLY_NAME("Alignment") { text_alignment::left };
};
}