#pragma once
#include <gl_core_4_5.h>
#include <utility> // swap

namespace ag {
namespace gl {
// Wrapper to use GLuint as a unique_ptr handle type
// http://stackoverflow.com/questions/6265288/unique-ptr-custom-storage-type-example/6272139#6272139
template <typename Deleter> struct GLHandle {
  unsigned int obj;
  GLHandle(unsigned int obj_) : obj{obj_} {}
  // default and nullptr constructors folded together
  GLHandle(std::nullptr_t = nullptr) : obj{0} {}

  GLHandle(GLHandle &&rhs) : obj{rhs.obj} { rhs.obj = 0; }
  GLHandle &operator=(GLHandle &&rhs) {
    std::swap(obj, rhs.obj);
    return *this;
  }

  GLHandle(const GLHandle &) = delete;
  GLHandle &operator=(const GLHandle &) = delete;

  ~GLHandle() {
    if (obj)
      Deleter{}(obj);
  }

  unsigned int get() const { return obj; }

  explicit operator bool() { return obj != 0; }
  friend bool operator==(const GLHandle &l, const GLHandle &r) {
    return l.obj == r.obj;
  }
  friend bool operator!=(const GLHandle &l, const GLHandle &r) {
    return !(l == r);
  }
  // default copy ctor and operator= are fine
  // explicit nullptr assignment and comparison unneeded
  // because of implicit nullptr constructor
  // swappable requirement fulfilled by std::swap
};
}
}

