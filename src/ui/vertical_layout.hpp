#pragma once
#include "visual.hpp"
#include <vector>

namespace ui {

class vertical_layout {
public:
  vertical_layout(const rect_2d &allocated_space)
      : allocated_space_{allocated_space} {}

  void add(visual &v) {
  	visuals_.push_back(&v);
  }

  void do_layout();

private:
  rect_2d allocated_space_;
  std::vector<visual *> visuals_;
};

}