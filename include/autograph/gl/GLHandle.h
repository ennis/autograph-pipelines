#pragma once
#include <autograph/Config.h>
#include <autograph/support/Span.h>
#include <gl_core_4_5.h>
#include <utility> // swap

namespace ag {
namespace gl {

struct GLObjectTrackingData {
  GLuint obj;
  GLenum type;
  uint64_t creationFrame;
  // TODO other data?
};

void trackGLObject(GLuint obj, GLenum type);
void releaseGLObject(GLuint obj, GLenum type);
int getGLObjectCount();
const GLObjectTrackingData *getGLObjectData(int index);

// Wrapper to use GLuint as a unique_ptr handle type
// http://stackoverflow.com/questions/6265288/unique-ptr-custom-storage-type-example/6272139#6272139
template <typename Deleter> struct GLHandle {
  unsigned int obj;
  GLHandle(GLuint obj_) : obj{obj_} {
#ifdef AG_TRACK_GL_OBJECTS
    trackGLObject(obj_, Deleter::objectType);
#endif
  }

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
    if (obj) {
#ifdef AG_TRACK_GL_OBJECTS
      releaseGLObject(obj, Deleter::objectType);
#endif
      Deleter{}(obj);
    }
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
