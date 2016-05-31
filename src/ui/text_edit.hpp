#pragma once
#include "button_behavior.hpp"

namespace ui {

namespace stb {
struct text_string {
  std::string &str;
  NVGcontext *ctx;
};
#define STB_TEXTEDIT_UNDOSTATECOUNT 99
#define STB_TEXTEDIT_UNDOCHARCOUNT 999
#define STB_TEXTEDIT_CHARTYPE char
#define STB_TEXTEDIT_POSITIONTYPE int
using STB_TEXTEDIT_STRING = text_string;
static constexpr char STB_TEXTEDIT_NEWLINE = '\n';
static constexpr float STB_TEXTEDIT_GETWIDTH_NEWLINE = -1.0f;
#include <stb_textedit.h>
}

class text_edit : public button_behavior {
public:
  text_edit(element &parent, std::string &str);

  glm::ivec2 measure(renderer &r) override {
    text_.ctx = r.nanovg();
    return glm::ivec2{200, 30};
  }

  void render(renderer &r) override;

  std::string &string() { return text_.str; }

  void process_input(const input::input_event &ev,
                     scheduler &event_sched) override;

private:
  std::unique_ptr<stb::STB_TexteditState> state_;
  stb::text_string text_;
  // TODO keep track 
};
}