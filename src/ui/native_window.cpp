#include "native_window.hpp"
#include <GLFW/glfw3.h>

namespace ui {

void native_window::render(renderer &r) {
  int w, h;
  glfwGetWindowSize(window_, &w, &h);
  rect_2d true_geom{{0, 0}, glm::ivec2{w, h}};
  r.render_native_window(window_, true_geom.size);

  for (auto p : children())
  {
	  if (p)
	  {
		  p->set_geometry(rect_2d{ true_geom.pos, p->content_size() });
		  p->render(r);
	  }
  }
}
}