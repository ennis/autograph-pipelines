#include "native_window.hpp"
#include <GLFW/glfw3.h>

namespace ui {

native_window::native_window(const glm::ivec2 &initial_size,
                             const std::string &title)
    : element{&ui::root_window()}, owns_window_{true}
       {
  window_ = glfwCreateWindow(initial_size.x, initial_size.y, title.c_str(),
                             nullptr, ui::root_window().get_window_impl());
}

native_window::~native_window() {
  if (owns_window_)
    glfwDestroyWindow(window_);
}

void native_window::render(renderer &r) {
  glfwMakeContextCurrent(window_);
  int w, h;
  glfwGetWindowSize(window_, &w, &h);
  rect_2d true_geom{{0, 0}, glm::ivec2{w, h}};
  r.render_native_window(window_, true_geom.size);

  /*for (auto p : children()) {
    if (p) {
      p->set_geometry(rect_2d{true_geom.pos, p->content_size()});
      p->render(r);
    }
  }*/
  // swapping buffers for the main window is handled by the main loop
  if (window_ != ui::root_window().get_window_impl())
    glfwSwapBuffers(window_);
  glfwMakeContextCurrent(ui::root_window().get_window_impl());
}

void native_window::fixed_update(scheduler &event_sched) {
  // container::fixed_update(event_sched);
  if (glfwWindowShouldClose(window_))
    // XXX ALWAYS use signal_deferred in UI event handlers
    // If the event handler resumes a coroutine that terminates
    // and happens to own the widget, then a synchronous call to signal()
    // is basically equivalent to 'delete this'. And it's bad.
    should_close.signal_deferred(event_sched);
}

void native_window::process_input(const input::input_event &ev,
                                  scheduler &event_sched) {
  /*for (auto p : children()) {
    if (p)
      // XXX should forward input to child only if it is visible
      p->process_input(ev, event_sched);
  }*/
}
}
