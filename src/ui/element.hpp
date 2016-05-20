#pragma once
#include "../rect.hpp"
#include <vector>
#include "visual.hpp"
#include "renderer.hpp"
#include <nanovg.h>

struct GLFWwindow;

namespace ui {
class element;
class native_window;
class style;

struct child_slot {
  element *parent;
  int slot;
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

  element *parent() {
	  return parent_;
  }

  void set_parent(element *parent)
  {
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
  glm::ivec2 content_size() const {
	  return content_size_;
  }

  virtual void render(renderer &r, const rect_2d &geometry) = 0;

private:
  element *parent_;
  int index_;
  glm::ivec2 content_size_;
};

class container : public element {
public:
  container(element *parent = nullptr) : element{parent} {}
  container(const child_slot &slot) : element{slot} {}

  const auto &children() const { return children_; }

  int add_child(element *elem) {
    children_.push_back(elem);
    return (int)(children_.size() - 1);
  }

  void add_child(element *elem, int slot) {
    if (slot < children_.size())
      return;
    children_[slot] = elem;
  }

  void remove_child(int slot) { children_[slot] = nullptr; }

  void remove_child(element *elem) {
    // TODO
  }

private:
  std::vector<ui::element *> children_;
};

native_window& initialize(GLFWwindow* root_window, NVGcontext* nvg_context);
native_window& root_window();
void render();

}