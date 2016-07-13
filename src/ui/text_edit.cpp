#include "native_window.hpp"
#include "text_edit.hpp"
#include <GLFW/glfw3.h>

namespace {
std::locale cloc{"C"};
}

namespace ui {

namespace stb {

int STB_TEXTEDIT_STRINGLEN(const text_string *obj) { return obj->str.size(); }
int STB_TEXTEDIT_GETCHAR(const text_string *obj, int idx) {
  return obj->str[idx];
}
bool STB_TEXTEDIT_IS_SPACE(char ch) { return std::isspace(ch, cloc); }

float STB_TEXTEDIT_GETWIDTH(text_string *obj, int line_start_idx,
                            int char_idx) {
  char c = obj->str[line_start_idx + char_idx];
  if (c == '\n')
    return STB_TEXTEDIT_GETWIDTH_NEWLINE;
  float bounds[4];
  char str[2] = {c, 0};
  float advance = nvgTextBounds(obj->ctx, 0, 0, str, nullptr, bounds);
  return advance;
}

void STB_TEXTEDIT_LAYOUTROW(StbTexteditRow *r, text_string *obj,
                            int line_start_idx) {
  const char *text = obj->str.c_str();

  float bounds[4];
  float advance =
      nvgTextBounds(obj->ctx, 0, 0, text + line_start_idx, nullptr, bounds);
  r->x0 = 0.0f;
  r->x1 = bounds[2];
  r->baseline_y_delta = bounds[3];
  r->ymin = 0.0f;
  r->ymax = bounds[3];
  r->num_chars = obj->str.size() - line_start_idx;
}

void STB_TEXTEDIT_DELETECHARS(STB_TEXTEDIT_STRING *str, int where, int len) {
  str->str.erase(where, len);
}

bool STB_TEXTEDIT_INSERTCHARS(STB_TEXTEDIT_STRING *str, int where,
                              const char *chars, int len) {
  str->str.insert(where, chars, len);
  return true;
}

#define TEXTEDIT_KEY_BASE 0x300000
#define STB_TEXTEDIT_K_LEFT (TEXTEDIT_KEY_BASE + GLFW_KEY_LEFT)
#define STB_TEXTEDIT_K_RIGHT (TEXTEDIT_KEY_BASE + GLFW_KEY_RIGHT)
#define STB_TEXTEDIT_K_UP (TEXTEDIT_KEY_BASE + GLFW_KEY_UP)
#define STB_TEXTEDIT_K_DOWN (TEXTEDIT_KEY_BASE + GLFW_KEY_DOWN)
#define STB_TEXTEDIT_K_LINESTART (TEXTEDIT_KEY_BASE + GLFW_KEY_HOME)
#define STB_TEXTEDIT_K_LINEEND (TEXTEDIT_KEY_BASE + GLFW_KEY_END)
#define STB_TEXTEDIT_K_TEXTSTART (TEXTEDIT_KEY_BASE + GLFW_KEY_LAST + 5)
#define STB_TEXTEDIT_K_TEXTEND (TEXTEDIT_KEY_BASE + GLFW_KEY_LAST + 6)
#define STB_TEXTEDIT_K_DELETE (TEXTEDIT_KEY_BASE + GLFW_KEY_DELETE)
#define STB_TEXTEDIT_K_BACKSPACE (TEXTEDIT_KEY_BASE + GLFW_KEY_BACKSPACE)
#define STB_TEXTEDIT_K_UNDO (TEXTEDIT_KEY_BASE + GLFW_KEY_LAST + 1)
#define STB_TEXTEDIT_K_REDO (TEXTEDIT_KEY_BASE + GLFW_KEY_LAST + 2)
#define STB_TEXTEDIT_K_WORDLEFT (TEXTEDIT_KEY_BASE + GLFW_KEY_LAST + 3)
#define STB_TEXTEDIT_K_WORDRIGHT (TEXTEDIT_KEY_BASE + GLFW_KEY_LAST + 4)
#define STB_TEXTEDIT_K_SHIFT 0x10000000

int STB_TEXTEDIT_KEYTOTEXT(int key) { return key >= 0x300000 ? 0 : key; }

#define STB_TEXTEDIT_IMPLEMENTATION
#include <stb_textedit.h>
}

text_edit::text_edit(element &parent, std::string &str)
    : text_{str, nullptr} {
  state_ = std::make_unique<stb::STB_TexteditState>();
}

void text_edit::process_input(const input::input_event &ev,
                              scheduler &event_sched) {
  // button_behavior::process_input(ev, event_sched);

  auto window = ui::root_window().get_window_impl();
  bool shift_state_ =
      (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) ||
      (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS);
  bool ctrl_state_ =
      (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS) ||
      (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS);

  if (auto keyEvent = ev.as<input::key_event>()) {
    switch (keyEvent->code) {
    case GLFW_KEY_LEFT:
      if (ctrl_state_)
        stb_textedit_key(&text_, state_.get(), STB_TEXTEDIT_K_WORDLEFT);

      break;
    case GLFW_KEY_RIGHT:
      if (ctrl_state_)
        stb_textedit_key(&text_, state_.get(), STB_TEXTEDIT_K_WORDRIGHT);
      break;
    case GLFW_KEY_Z:
      if (ctrl_state_)
        if (shift_state_)
          stb_textedit_key(&text_, state_.get(), STB_TEXTEDIT_K_REDO);
        else
          stb_textedit_key(&text_, state_.get(), STB_TEXTEDIT_K_UNDO);
      break;
    default:
      break;
    }
  } else if (auto mouseButtonEvent = ev.as<input::mouse_button_event>()) {
    auto pos = input::cursor_pos();
    auto relpos = geometry().relative(pos);
    if (mouseButtonEvent->button == 0 &&
        mouseButtonEvent->state == input::button_state::pressed)
      if (geometry().inside(pos))
        stb_textedit_click(&text_, state_.get(), (float)relpos.x,
                           (float)relpos.y);
  } else if (auto textEvent = ev.as<input::text_event>()) {
    stb_textedit_key(&text_, state_.get(), textEvent->codepoint);
  }
}

struct text_edit_line {
  int begin_index;
  int end_index; // in bytes
  // updated on render
  float xmin, xmax;
};

int char_from_utf8(unsigned int *out_char, const char *in_text,
                   const char *in_text_end) {
  unsigned int c = (unsigned int)-1;
  const unsigned char *str = (const unsigned char *)in_text;
  if (!(*str & 0x80)) {
    c = (unsigned int)(*str++);
    *out_char = c;
    return 1;
  }
  if ((*str & 0xe0) == 0xc0) {
    *out_char = 0xFFFD; // will be invalid but not end of string
    if (in_text_end && in_text_end - (const char *)str < 2)
      return 1;
    if (*str < 0xc2)
      return 2;
    c = (unsigned int)((*str++ & 0x1f) << 6);
    if ((*str & 0xc0) != 0x80)
      return 2;
    c += (*str++ & 0x3f);
    *out_char = c;
    return 2;
  }
  if ((*str & 0xf0) == 0xe0) {
    *out_char = 0xFFFD; // will be invalid but not end of string
    if (in_text_end && in_text_end - (const char *)str < 3)
      return 1;
    if (*str == 0xe0 && (str[1] < 0xa0 || str[1] > 0xbf))
      return 3;
    if (*str == 0xed && str[1] > 0x9f)
      return 3; // str[1] < 0x80 is checked below
    c = (unsigned int)((*str++ & 0x0f) << 12);
    if ((*str & 0xc0) != 0x80)
      return 3;
    c += (unsigned int)((*str++ & 0x3f) << 6);
    if ((*str & 0xc0) != 0x80)
      return 3;
    c += (*str++ & 0x3f);
    *out_char = c;
    return 3;
  }
  if ((*str & 0xf8) == 0xf0) {
    *out_char = 0xFFFD; // will be invalid but not end of string
    if (in_text_end && in_text_end - (const char *)str < 4)
      return 1;
    if (*str > 0xf4)
      return 4;
    if (*str == 0xf0 && (str[1] < 0x90 || str[1] > 0xbf))
      return 4;
    if (*str == 0xf4 && str[1] > 0x8f)
      return 4; // str[1] < 0x80 is checked below
    c = (unsigned int)((*str++ & 0x07) << 18);
    if ((*str & 0xc0) != 0x80)
      return 4;
    c += (unsigned int)((*str++ & 0x3f) << 12);
    if ((*str & 0xc0) != 0x80)
      return 4;
    c += (unsigned int)((*str++ & 0x3f) << 6);
    if ((*str & 0xc0) != 0x80)
      return 4;
    c += (*str++ & 0x3f);
    // utf-8 encodings of values used in surrogate pairs are invalid
    if ((c & 0xFFFFF800) == 0xD800)
      return 4;
    *out_char = c;
    return 4;
  }
  *out_char = 0;
  return 0;
}

struct text_edit_state {
  std::string text;
  // updated on render
  float line_height;
  std::vector<text_edit_line> lines;
  // internals
  std::unique_ptr<stb::STB_TexteditState> stbstate_;

  void update_lines(NVGcontext *vg, float lineheight, float wrap_width,
                    bool word_wrap) {
    line_height = lineheight;

    lines.clear();

    int line_begin = 0;
    int word_begin = 0;
    int word_begin_skip_ws = 0;

    auto measure = [this, vg](int beg, int end) -> float {
      float bounds[4];
      auto ptr = this->text.c_str();
      float advance = nvgTextBounds(vg, 0, 0, ptr + beg, ptr + end, bounds);
      return advance;
    };

    auto add_line = [&](int beg, int end) {
      float m = measure(beg, end);
      text_edit_line l{beg, end, 0.0f, m};
      this->lines.push_back(l);
    };

    auto space = [](char32_t ch) -> bool { return std::isspace(ch, cloc); };

    auto space_or_newline = [](char32_t ch) -> bool {
      return std::isspace(ch, cloc) || ch == '\n';
    };

    auto getcp = [this](int pos, int &len) -> char32_t {
      unsigned int out;
      const auto ptr = this->text.c_str();
      auto l = this->text.size();
      len = char_from_utf8(&out, ptr + pos, ptr + l);
      return (char32_t)out;
    };

    // while there is still text
    // if word wrapping: read word, append to current linebuf
    // else: add to linebuf until '\n'

    auto len = text.size();
    int i = 0;
    int nb = 0;
    while (i < len) {
      line_begin = i;
      if (word_wrap) {
        // skip leading whitespace
        word_begin = i;
        while (i < len && space(getcp(i, nb))) {
          i += nb;
        }
        if (i == len)
          break;
        if (text[i] == '\n') {
          add_line(line_begin, i);
          continue;
        }
        word_begin_skip_ws = i;
        while (i < len && !space_or_newline(getcp(i, nb))) {
          i += nb;
        }
        if (i == len)
          break;
        if (measure(line_begin, i) >= wrap_width) {
          if (line_begin == word_begin) {
            // cannot split a word, break here, but skip
            add_line(line_begin, i);
            while (i < len && space(getcp(i, nb))) {
              i += nb;
            }
          } else {
            add_line(line_begin, word_begin);
            i = word_begin_skip_ws;
          }
        }
      } else {
        while (i < len && getcp(i, nb) != '\n') {
          i += nb;
        }
        if (i == len)
          break;
        add_line(line_begin, i);
      }
    }
    // add the current line
    // this will add a zero-length line at the end if line_begin == i
    add_line(line_begin, i);
  }
};

void text_edit::render(renderer &r) {
  text_.ctx = r.nanovg();
}
}
