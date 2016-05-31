#pragma once
#include "../image_desc.hpp"
#include "sprite.hpp"
#include "visual_state.hpp"
#include <nanovg.h>

struct GLFWwindow;

namespace ui {

class renderer {
public:
	struct text_edit_state
	{
		std::string& str;
		int sel_begin;
		int sel_end;
		int caret_pos;
	};

  renderer(NVGcontext *ctx, GLFWwindow *root_window);

  virtual ~renderer();

  void push_clip(const rect_2d &rect);
  rect_2d get_clip();
  void pop_clip();

  virtual glm::ivec2 measure_button(const std::string &label, ui::sprite *icon);

  virtual void render_button(const rect_2d &geometry, const std::string &label,
                             ui::sprite *icon, visual_state state);

  virtual void measure_native_window(GLFWwindow *window_,
                                     glm::ivec2 contents_size);

  virtual void render_native_window(GLFWwindow *window_,
                                    glm::ivec2 contents_size);

  virtual glm::ivec2 measure_slider();
  virtual void render_slider_float(const rect_2d &geometry, float min,
                                   float max, float cur);

  void render_text_edit(const rect_2d &geometry, const text_edit_state& state);

  //void measure_text_edit(std::string& str);

  virtual void end_frame();

  NVGcontext *nanovg() { return ctx_; }

private:
  std::vector<rect_2d> clip_stack_;
  NVGcontext *ctx_{nullptr};
  int ui_font_{0};
  int ui_font_size_{24};
  GLFWwindow *cur_native_window_{nullptr};
};
}