#pragma once
#include "../image_desc.hpp"
#include <glm/glm.hpp>
#include "size_policy.hpp"

namespace ui {

//
// A visual represents a rectangular region of the screen
class visual {
public:
  visual() {}
  virtual ~visual() {}

  const rect_2d& geometry() const { return geometry_; }
  void set_geometry(const rect_2d &geom) { geometry_ = geom; }

  // can be overriden for non-rectangular geometry
  virtual bool hit_test(glm::ivec2 pos) const {
  	return geometry_.inside(pos);
  }

private:
  rect_2d geometry_;
};
}