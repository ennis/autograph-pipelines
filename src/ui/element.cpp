#include "element.hpp"
#include "native_window.hpp"
#include "renderer.hpp"

namespace ui {
element::element(element *parent) : parent_{parent} {
  if (parent_)
    index_ = parent->add_child(this);
}
element::element(const child_slot &slot) {}

element::~element() {
  if (parent_)
    parent_->remove_child(index_);
}

namespace {
struct ui_state {
  std::unique_ptr<native_window> root_window_;
  std::unique_ptr<renderer> renderer_;
  subscription input_sub_;
};

std::unique_ptr<ui_state> g_ui_state;
}

native_window &initialize(GLFWwindow *root_window, NVGcontext *nvg_context) {
  g_ui_state = std::make_unique<ui_state>();
  g_ui_state->root_window_ = std::make_unique<native_window>(root_window);
  g_ui_state->renderer_ = std::make_unique<renderer>(nvg_context, root_window);
  input::events.subscribe(g_ui_state->input_sub_, [](auto ev) {
	  process_input(*ev);
  });
  return *g_ui_state->root_window_;
}

native_window &root_window() { return *g_ui_state->root_window_; }

void process_input(const input::input_event& ev)
{
	scheduler sched;
	root_window().process_input(ev, sched);
	sched.execute();
}

void render() {
  auto &root_win = root_window();
  auto win_content_size = root_win.cache_measure(*g_ui_state->renderer_);
  root_win.set_geometry(rect_2d{ { 0, 0 }, win_content_size });
  root_win.render(*g_ui_state->renderer_);
}


}
