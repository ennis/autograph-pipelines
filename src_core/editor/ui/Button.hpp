#pragma once
#include <cppformat/format.h>
#include <editor/Input.hpp>
#include <editor/RectTransform.hpp>
#include <editor/Scene.hpp>
#include <editor/ui/DefaultStyle.hpp>
#include <editor/ui/Widget.hpp>
#include <reflection/reflect.hpp>

namespace ui {

class REFLECT Button : public Widget {
public:
  using Visual = std::function<void(SkCanvas &, State, glm::ivec2)>;

  Button() = default;

  virtual bool onPointerUp() { 
	  Widget::onPointerUp();
	  onClick();
	  return true;
  }

  virtual bool onPointerEnter() {
	  Widget::onPointerEnter();
	  fmt::print("onPointerEnter {}\n", (intptr_t)this);
	  return true;
  }

  virtual bool onPointerExit() {
	  Widget::onPointerExit();
	  fmt::print("onPointerExit {}\n", (intptr_t)this);
	  return true;
  }


  void render(HUDRenderer &renderer, glm::vec2 size) override {
    visual(*renderer.canvas, state, size);
  }

  Observable<void> onClick M_FRIENDLY_NAME("On click");

  // visuals
  Visual visual{default_style::renderButton};
};

}
