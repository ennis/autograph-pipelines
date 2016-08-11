#pragma once
#include "../entity.hpp"
#include <cppformat/format.h>
#include <reflect.hpp>

namespace ui {

struct layout_component : public component<layout_component> {
  glm::vec2 content_size;
  float rel_width;
  float rel_height;
};

struct layout_controller : public behaviour<layout_controller> {
  virtual ~layout_controller() {}

  auto &children() { return get_component<rect_transform>()->children; }

  virtual void measure_contents() = 0;
  virtual void layout_contents() = 0;

  glm::vec2 preferred_size;
};

struct vertical_layout_controller : public layout_controller {
  float spacing = 2.0f;
  float padding_left = 0.0f;
  float padding_right = 0.0f;
  float padding_top = 0.0f;
  float padding_bottom = 0.0f;

  void measure_contents() override {
    float height = padding_top + padding_bottom + children().size() * spacing;
    float width = 0.0f;
    for (auto &&c : children()) {
      if (auto layout = c->get_component<layout_controller>()) {
        width = std::max(width, layout->preferred_size.x);
        height += layout->preferred_size.y;
      } else
        height += 10.0f;
    
    }
    preferred_size.x = width;
    preferred_size.y = height;
  }

  void layout_contents() override {
    int num_children = children().size();
    float div = 1.0f / static_cast<float>(num_children);
    int i = 0;
    for (auto &&c : children()) {
      if (auto tr = c->get_component<rect_transform>()) {
        float h = i * div;
        // override user values
        // fix anchor
        tr->anchor_a = glm::vec2{0.0f, h};
        tr->anchor_b = glm::vec2{1.0f, h + div};
        tr->offset_a = glm::vec2{padding_left, 0.0f};
        tr->offset_b = glm::vec2{-padding_right, -spacing};
        /*fmt::print("- layout child {} = anchor {},{},{},{} offset
           {},{},{},{}\n", i,
                tr->anchor_a.x, tr->anchor_a.y, tr->anchor_b.x, tr->anchor_b.y,
                tr->offset_a.x, tr->offset_a.y, tr->offset_b.x,
           tr->offset_b.y);*/
        ++i;
      }
    }
  }
};

struct grid_layout_controller : public layout_controller {
  grid_layout_controller() = default;
  grid_layout_controller(int num_rows_, int num_columns_,
                         float horizontal_spacing_ = 2.0f,
                         float vertical_spacing_ = 2.0f)
      : horizontal_spacing{horizontal_spacing_},
        vertical_spacing{vertical_spacing_}, num_rows{num_rows_},
        num_columns{num_columns_} {}

  float horizontal_spacing = 2.0f;
  float vertical_spacing = 2.0f;
  int num_rows = 5;
  int num_columns = 5;

  void measure_contents() override {}

  void layout_contents() override {
    float xdiv = 1.0f / static_cast<float>(num_columns);
    float ydiv = 1.0f / static_cast<float>(num_rows);
    int i = 0, j = 0;
    for (auto &&c : children()) {
      if (auto tr = c->get_component<rect_transform>()) {
        float x = j * xdiv;
        float y = i * xdiv;
        // override user values
        // fix anchor
        tr->anchor_a = glm::vec2{x, y};
        tr->anchor_b = glm::vec2{x + xdiv, y + ydiv};
        tr->offset_a = glm::vec2{0.0f, 0.0f};
        tr->offset_b = glm::vec2{-horizontal_spacing, -vertical_spacing};
        /*fmt::print("- layout child {} = anchor {},{},{},{} offset
           {},{},{},{}\n", i,
                tr->anchor_a.x, tr->anchor_a.y, tr->anchor_b.x, tr->anchor_b.y,
                tr->offset_a.x, tr->offset_a.y, tr->offset_b.x,
           tr->offset_b.y);*/

        ++j;
        if (j >= num_columns) {
          j = 0;
          ++i;
        }
      }
    }
  }
};
}
