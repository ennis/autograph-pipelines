#pragma once
#include <autograph/Gfx/Exports.h>
#include <autograph/Gfx/gl_core_4_5.h>
#include <autograph/Core/Support/Span.h>
#include <utility> // swap

namespace ag {

struct GLObjectTrackingData {
  gl::GLuint obj;
  gl::GLenum type;
  int64_t creationFrame;
};

AG_GFX_API void trackGLObject(gl::GLuint obj, gl::GLenum type);
AG_GFX_API void releaseGLObject(gl::GLuint obj, gl::GLenum type);
AG_GFX_API int getGLObjectCount();
AG_GFX_API const GLObjectTrackingData *getGLObjectData(int index);
AG_GFX_API const char *getGLObjectTypeName(gl::GLenum type);

/// Wrapper to use gl::GLuint as a unique_ptr handle type
/// http://stackoverflow.com/questions/6265288/unique-ptr-custom-storage-type-example/6272139#6272139
/// This type follows the same semantics as unique_ptr.
/// Deleter is a functor type with a static operator()(GLuint) member, that is in charge of deleting the OpenGL object 
/// by calling glDeleteX
/// The deleter is automatically called when obj != 0 and: 
///		- the GLHandle goes out of scope 
///		- the GLHandle is move-assigned another GLHandle 
template <typename Deleter> struct GLHandle 
{
  unsigned int obj;
  GLHandle(gl::GLuint obj_) : obj{obj_} {
#ifdef AG_TRACK_GL_OBJECTS
    trackGLObject(obj_, Deleter::objectType);
#endif
  }

  /// default and nullptr constructors folded together
  GLHandle(std::nullptr_t = nullptr) : obj{0} {}

  /// Move-constructor: take ownership of the GL resource
  GLHandle(GLHandle &&rhs) : obj{rhs.obj} { rhs.obj = 0; }
  /// Move-assignment operator: take ownership if the GL resource
  GLHandle &operator=(GLHandle &&rhs) {
    std::swap(obj, rhs.obj);
    return *this;
  }

  // GL resources are not copyable, so delete the copy ctors
  GLHandle(const GLHandle &) = delete;
  GLHandle &operator=(const GLHandle &) = delete;

  ~GLHandle() {
    if (obj) {
#ifdef AG_TRACK_GL_OBJECTS
      releaseGLObject(obj, Deleter::objectType);
#endif
      Deleter{}(obj);
	  obj = 0;
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
