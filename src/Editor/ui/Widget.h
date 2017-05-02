#pragma once
#include <autograph/Engine/Input.h>
#include <autograph/RectTransform.h>
#include <autograph/Types.h>
#include <vector>
#include <memory>

class SkCanvas;

namespace ag {
namespace ui {

template <typename T> class optional_ptr {
public:
  optional_ptr() = default;
  optional_ptr(T *p) : _p(p), _own(false) {}
  optional_ptr(std::unique_ptr<T> p) : _p(p.release()), _own(true) {}
  ~optional_ptr() {
    if (_own)
      delete _p;
  }

  optional_ptr(const optional_ptr<T> &) = delete;
  optional_ptr<T> &operator=(const optional_ptr<T> &) = delete;

  optional_ptr(optional_ptr<T> &&rhs) noexcept {
    if (_own)
      delete _p;
    _p = rhs._p;
    _own = rhs._own;
    rhs._p = nullptr;
    rhs._own = false;
  }

  optional_ptr<T> &operator=(optional_ptr<T> &&rhs) noexcept {
    _p = rhs._p;
    _own = rhs._own;
    rhs._p = nullptr;
    rhs._own = false;
    return *this;
  }

  bool owns() const { return _own; }
  T *get() const { return _p; }
  T &operator*() const { return *_p; }
  T *operator->() const { return _p; }
  T *release() noexcept {
    auto *p = _p;
    _own = false;
    _p = nullptr;
    return p;
  }
  void reset(T *p) {
    if (_own)
      delete _p;
    _p = p;
    _own = false;
  }
  void reset(std::unique_ptr<T> p) {
    if (_own)
      delete _p;
    _p = p.release();
    _own = true;
  }

private:
  T *_p = nullptr;
  bool _own = false;
};

/**
 * @brief A widget in the visual tree
 * @details [long description]
 *
 */
class Widget {
public:
  using OptionalPtr = optional_ptr<Widget>;

  virtual ~Widget() {
    // if parented, orphan
    // NOTE: if this widget is owned by another widget,
    // then parent will be set to zero before entering this destructor
    if (parent)
      parent->orphanChild(this);
    // orphan all children
    for (auto &&c : children) {
      c->parent = nullptr;
    }
  }

  friend class UICanvas;

  /**
   * @brief [brief description]
   * @details [long description]
   *
   */
  class Layout {
  public:
    virtual void measureContents(Widget &) = 0;
    virtual void layoutContents(Widget &) = 0;
  };

  /**
   * @brief [brief description]
   * @details [long description]
   * @return [description]
   */
  virtual bool onPointerDown(vec2 pos) { return false; }

  /**
   * @brief [brief description]
   * @details [long description]
   * @return [description]
   */
  virtual bool onPointerUp(vec2 pos) { return false; }

  /**
   * @brief [brief description]
   * @details [long description]
   * @return [description]
   */
  virtual bool onPointerEnter(vec2 pos) { return false; }

  /**
   * @brief [brief description]
   * @details [long description]
   * @return [description]
   */
  virtual bool onPointerExit(vec2 pos) { return false; }

  /**
   * @brief [brief description]
   * @details [long description]
   * @return [description]
   */
  virtual bool onSelect() { return false; }

  /**
   * @brief [brief description]
   * @details [long description]
   * @return [description]
   */
  virtual bool onDeselect() { return false; }

  /**
   * @brief [brief description]
   * @details [long description]
   *
   * @param visual [description]
   */
  virtual void render(SkCanvas &canvas) {}

  enum class VisualState { Default, Focused, Active, Disabled };

  // might make 'child' invalid if it is owned
  void orphanChild(Widget *child) {
    child->parent = nullptr;
    // TODO do this lazily
    children.erase(std::remove_if(children.begin(), children.end(),
                                  [child](auto &&c) {
                                    return !c.owns() && (c.get() == child);
                                  }),
                   children.end());
  }

  auto &getChildren() { return children; }

  // add child and take ownership
  void add(std::unique_ptr<Widget> w) {
    w->setParent(this);
    children.push_back(OptionalPtr{std::move(w)});
  }

  // add reference to heap-allocated widget
  void add(std::unique_ptr<Widget> &w) {
    w->setParent(this);
    children.push_back(OptionalPtr{w.get()});
  }

  void add(Widget &w) {
    w.setParent(this);
    children.push_back(OptionalPtr{&w});
  }

  // add stuff
  template <typename... Content> void addMany(Content &&... contents) {
    auto dummy = {(add(std::forward<Content>(contents)), 0)...};
  }

  // optional layout
  std::unique_ptr<Layout> layout{ nullptr };
  RectTransform rectTransform;
  RectTransform::Calculated calculatedTransform;
  vec2 preferredSize;
  VisualState visualState{ VisualState::Default };

protected:
  std::vector<OptionalPtr> children;
  Widget *parent{nullptr};

private:
  // Should fail for owned widgets
  void setParent(Widget *newParent) {
    if (parent != nullptr) {
      throw std::logic_error("Widget already has a parent");
    }
    parent = newParent;
    /*if (parent == newParent)
            return;
    if (parent)
            parent->orphanChild(this);
    parent = newParent;*/
  }
};
}
}