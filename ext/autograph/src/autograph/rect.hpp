#ifndef RECT_HPP
#define RECT_HPP

namespace ag {
struct Box1D {
  unsigned xmin;
  unsigned xmax;

  constexpr unsigned width() const { return xmax - xmin; }
};

struct Box2D {
  unsigned xmin;
  unsigned ymin;
  unsigned xmax;
  unsigned ymax;

  constexpr unsigned width() const { return xmax - xmin; }
  constexpr unsigned height() const { return ymax - ymin; }
};

struct Box3D {
  unsigned xmin;
  unsigned ymin;
  unsigned zmin;
  unsigned xmax;
  unsigned ymax;
  unsigned zmax;

  constexpr unsigned width() const { return xmax - xmin; }
  constexpr unsigned height() const { return ymax - ymin; }
  constexpr unsigned depth() const { return zmax - zmin; }
};
}

#endif
