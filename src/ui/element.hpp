#pragma once
#include "../rect.hpp"
#include "input.hpp"
#include "renderer.hpp"
#include <nanovg.h>
#include <vector>

struct GLFWwindow;

namespace ui {
class element;
class native_window;
class style;

struct child_slot {
  element *parent;
  int slot;
};

//
// optionally owning pointer
// construct from unique_ptr or rvalue ref: it's owned
// construct from T* or T&: it's a reference
template <typename T> class elem_ref {
public:
  elem_ref() : owned_{false} { u.ptr_ = nullptr; }

  elem_ref(std::unique_ptr<T> v) : owned_{true} { u.value_ = std::move(v); }

  elem_ref(T &&v) : elem_ref{std::make_unique<T>(std::move(v))} {}

  elem_ref(T &ref) : owned_{false} { u.ptr_ = &ref; }

  ~elem_ref() {
    if (owned_) {
      u.value_.~unique_ptr();
    }
  }

  bool is_owned() const { return owned_; }

  T *get() {
    if (owned_)
      return u.value_.get();
    else
      return u.ptr_;
  }

  /*static elem_ptr<T> make_owned(T &&v) {
    return elem_ptr<T>(std::make_unique<T>(std::move(v)));
  }

  static elem_ptr<T> make_ref(T *v) { return elem_ptr<T>(v); }
  static elem_ptr<T> make_ref(T &v) { return elem_ptr<T>(&v); }*/

private:
  bool owned_;
  union U {
    U() : value_{nullptr} {}

    ~U() {}

    std::unique_ptr<T> value_;
    T *ptr_;
  } u;
};

class element {
public:
  //
  // element constructor
  // If parent != nullptr, then the element will automatically add itself as a
  // child of parent
  // this should be a cheap operation
  element(element *parent = nullptr);
  element(const child_slot &slot);

  //
  // element destructor
  // if parent_!=nullptr, the element will remove itself from the children
  // of parent_
  // this should be a cheap operation (no vector resize or scan)
  virtual ~element();

  element *parent() { return parent_; }

  void set_parent(element *parent) {
    if (parent_)
      parent_->remove_child(index_);
    parent_ = parent;
  }

  virtual int add_child(element *elem) { return -1; }
  virtual void add_child(element *elem, int slot) {}
  virtual void remove_child(int slot) {}
  virtual void remove_child(element *elem) {}

  glm::ivec2 cache_measure(renderer &r) { return content_size_ = measure(r); }
  virtual glm::ivec2 measure(renderer &r) = 0;
  virtual void process_input(const input::input_event &ev,
                             scheduler &event_sched) {}

  // called once per frame for each widget
  // whether it is visible or not
  virtual void fixed_update(scheduler &event_sched) {}

  glm::ivec2 content_size() const { return content_size_; }

  void set_geometry(const rect_2d &geom) { geometry_ = geom; }
  const rect_2d &geometry() const { return geometry_; }

  virtual void render(renderer &r) = 0;

private:
  element *parent_;
  int index_;
  glm::ivec2 content_size_;
  rect_2d geometry_;
};

native_window &initialize(GLFWwindow *root_window, NVGcontext *nvg_context);
native_window &root_window();
void render();
void process_input(const input::input_event &ev);
}
