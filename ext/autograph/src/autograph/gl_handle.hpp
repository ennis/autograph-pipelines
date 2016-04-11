#ifndef GL_HANDLE_HPP
#define GL_HANDLE_HPP

#include "gl_types.hpp"
#include <utility> // swap

namespace ag {
// Wrapper to use GLuint as a unique_ptr handle type
// http://stackoverflow.com/questions/6265288/unique-ptr-custom-storage-type-example/6272139#6272139
// TODO move this in a shared header
template <typename Deleter> struct gl_handle {
  GLuint obj;
  gl_handle(GLuint obj_) : obj(obj_) {}
  // default and nullptr constructors folded together
  gl_handle(std::nullptr_t = nullptr) : obj(0) {}

  gl_handle(gl_handle &&rhs) : obj(rhs.obj) { rhs.obj = 0; }
  gl_handle &operator=(gl_handle &&rhs) {
    std::swap(obj, rhs.obj);
    return *this;
  }

  gl_handle(const gl_handle &) = delete;
  gl_handle &operator=(const gl_handle &) = delete;

  ~gl_handle() {
    if (obj)
      Deleter{}(obj);
  }

  GLuint get() const { return obj; }

  explicit operator bool() { return obj != 0; }
  friend bool operator==(const gl_handle &l, const gl_handle &r) {
    return l.obj == r.obj;
  }
  friend bool operator!=(const gl_handle &l, const gl_handle &r) {
    return !(l == r);
  }
  // default copy ctor and operator= are fine
  // explicit nullptr assignment and comparison unneeded
  // because of implicit nullptr constructor
  // swappable requirement fulfilled by std::swap
};
}

#endif
