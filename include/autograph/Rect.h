#pragma once
#include <autograph/Types.h>

namespace ag
{
//////////////////////////////////////////////
struct Rect2D 
{
  vec2 position;
  vec2 size;

  vec2 topLeft() const { return position; }
  vec2 topRight() const { return vec2{position.x + size.x, position.y}; }
  vec2 bottomLeft() const { return vec2{position.x, position.y + size.y}; }
  vec2 bottomRight() const { return vec2{position.x + size.x, position.y + size.y}; }

  constexpr bool inside(vec2 p) const {
    return (p.x <= position.x + size.x) && (p.x >= position.x) &&
           (p.y <= position.y + size.y) && (p.y >= position.y);
  }
};
}