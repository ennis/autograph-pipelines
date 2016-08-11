#pragma once
#include <cppformat/format.h>
#include <nanovg.h>
#include <reflect.hpp>
#include "../behavior.hpp"
#include "../input.hpp"
#include "../rect_transform.hpp"
#include "../vector_sprite.hpp"
#include "../scene.hpp"
#include "visual.hpp"

namespace REFLECT ui {

// default button visuals
auto button_visual_pressed = [](NVGcontext *vg, glm::vec2 size) {
	constexpr float sink = 5.0f;
  nvgBeginPath(vg);
  nvgRect(vg, 0.5f + sink, 0.5f + sink, size.x - sink, size.y - sink);
  nvgFillColor(vg, nvgRGBA(0, 200, 0, 255));
  nvgFill(vg);
};

// default button visuals
auto button_visual_released = [](NVGcontext *vg, glm::vec2 size) {
	constexpr float sink = 5.0f;
  nvgBeginPath(vg);
  nvgRect(vg, 0.5f + sink, 0.5f + sink, size.x - sink, size.y - sink);
  nvgFillColor(vg, nvgRGBA(10, 10, 10, 255));
  nvgFill(vg);
  nvgBeginPath(vg);
  nvgRect(vg, 0.5f, 0.5f, size.x - sink, size.y - sink);
  nvgFillColor(vg, nvgRGBA(0, 200, 0, 255));
  nvgFill(vg);
};

struct selectable : public behaviour<selectable> {
  virtual ~selectable() {}

  virtual bool on_pointer_down() { return true; }

  selectable *select_left = nullptr;
  selectable *select_right = nullptr;
  selectable *select_down = nullptr;
  selectable *select_up = nullptr;
};

struct button : public selectable {
  button() = default;
  button(vector_visual::callback_t pressed_visual_,
         vector_visual::callback_t released_visual_)
      : pressed_visual{std::move(pressed_visual_)},
        released_visual{std::move(released_visual_)} {}

  enum class state { pressed, released };

  void start() {
    fmt::print("button::start()\n");
    // scene update event
    // connect_member(get_scene().update, &button::on_update);
  }

  void on_update() { fmt::print("button::update()\n"); }

  bool on_pointer_down() override {
    fmt::print("button::on_pointer_down()\n");
    pressed.signal();
    return true;
  }

  observable<void> pressed;
  observable<void> released;

  vector_visual::callback_t pressed_visual;
  vector_visual::callback_t released_visual;
  state current_state = state::released;
};
}
