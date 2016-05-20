#include "renderer.hpp"
#define BLENDISH_IMPLEMENTATION
#include "../project_root.hpp"
#include "blendish.h"
#include "ruler.hpp"
#include <GLFW/glfw3.h>

namespace ui {
renderer::renderer(NVGcontext *ctx, GLFWwindow *root_window) : ctx_{ctx} {
  // load default font
  auto fontpath = project_root() / "Roboto-Regular.ttf";
  auto fontstr = fontpath.string();
  ui_font_ = nvgCreateFont(ctx_, "Roboto", fontstr.c_str());
  bndSetFont(ui_font_);
}

renderer::~renderer() {}

void renderer::push_clip(const rect_2d &rect) {}

rect_2d renderer::get_clip() { return rect_2d{}; }

void renderer::pop_clip() {}

glm::ivec2 measure_text(NVGcontext *ctx, const std::string &str) {
  float bounds[4];
  // float advance = nvgTextBounds(ctx, 0, 0, str.c_str(), nullptr, bounds);
  return glm::ivec2{(int)bounds[2] - (int)bounds[0],
                    (int)bounds[3] - (int)bounds[1]};
}

glm::ivec2 renderer::measure_button(const std::string &label,
                                    ui::sprite *icon) {
  ruler r;
  float w = bndLabelWidth(ctx_, -1, label.c_str());
  r.append(glm::ivec2{(int)w, (int)bndLabelHeight(ctx_, -1, label.c_str(), w)});
  if (icon)
    r.append(icon->size());
  r.set_min_height(BND_WIDGET_HEIGHT);
  return r.size();
}

void renderer::render_button(const rect_2d &geometry, const std::string &label,
                             ui::sprite *icon, visual_state state) {
  push_clip(geometry);
  int bnd_state = BND_DEFAULT;
  switch (state) {
  case visual_state::active:
    bnd_state = BND_ACTIVE;
    break;
  case visual_state::default_:
    bnd_state = BND_DEFAULT;
    break;
  case visual_state::hovered:
    bnd_state = BND_HOVER;
    break;
  case visual_state::inactive:
  default:
    break;
  }
  bndToolButton(ctx_, geometry.pos.x, geometry.pos.y, geometry.size.x,
                geometry.size.y, BND_CORNER_NONE, BND_DEFAULT, -1,
                label.c_str());
  pop_clip();
}

void renderer::measure_native_window(GLFWwindow *window,
                                     glm::ivec2 contents_size) {}

// note: this is called before rendering any children
void renderer::render_native_window(GLFWwindow *window,
                                    glm::ivec2 contents_size) {
  if (cur_native_window_) {
    int w, h;
    glfwGetWindowSize(cur_native_window_, &w, &h);
    gl::Viewport(0, 0, w, h);
    nvgEndFrame(ctx_);
  }

  int w, h;
  glfwGetWindowSize(window, &w, &h);
  nvgBeginFrame(ctx_, w, h, 1.0f);
  cur_native_window_ = window;
}

glm::ivec2 renderer::measure_slider() {
  return glm::ivec2{300, BND_WIDGET_HEIGHT};
}

void renderer::render_slider_float(const rect_2d &geometry, float min,
                                   float max, float cur) {
  auto p = (cur - min) / (max - min);
  auto valstr = std::to_string(cur);
  bndSlider(ctx_, geometry.pos.x, geometry.pos.y, geometry.size.x,
            geometry.size.y, BND_CORNER_NONE, BND_DEFAULT, p, nullptr,
            valstr.c_str());
}

void renderer::end_frame() {
  if (cur_native_window_) {
    int w, h;
    glfwGetWindowSize(cur_native_window_, &w, &h);
    gl::Viewport(0, 0, w, h);
    nvgEndFrame(ctx_);
  }
}
}
