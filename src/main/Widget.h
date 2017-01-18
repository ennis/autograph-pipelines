#pragma once
#include <algorithm>
#include <autograph/Types.h>
#include <autograph/engine/Application.h>
#include <memory>
#include <vector>

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

class Widget;
using OptionalPtr = optional_ptr<Widget>;

struct WidgetList {
  WidgetList() {}

  template <typename... Widgets> explicit WidgetList(Widgets &&... widgets) {
    // move into list_;
    auto dummy = {0, (add(std::forward<Widgets>(widgets)), 0)...};
  }

  WidgetList(WidgetList &&) = default;
  WidgetList &operator=(WidgetList &&) = default;
  WidgetList(const WidgetList &) = delete;
  WidgetList &operator=(const WidgetList &) = delete;

  auto add(OptionalPtr ptr) {
    auto p = ptr.get();
    list_.push_back(std::move(ptr));
    return p;
  }

  // add child and take ownership
  auto add(std::unique_ptr<Widget> w) -> Widget * {
    return add(OptionalPtr{std::move(w)});
  }

  // add reference to heap-allocated widget
  auto add(std::unique_ptr<Widget> &w) { return add(OptionalPtr{w.get()}); }

  // add reference to widget on stack or heap
  // w should not move afterwards
  auto add(Widget &w) { return add(OptionalPtr{&w}); }

  // fun fact: not adding the is_base_of test will freeze clang
  template <typename T,
            typename = std::enable_if_t<std::is_rvalue_reference<T &&>::value &&
                                        std::is_base_of<Widget, T>::value>>
  auto add(T &&widget) -> Widget * {
    return add(std::make_unique<T>(std::move(widget)));
  }

  std::vector<OptionalPtr> list_;
};

struct Content : public WidgetList {
  using WidgetList::WidgetList;
};

/**
 * @brief A widget in the visual tree
 * @details [long description]
 *
 */
class Widget {
public:
  Widget() {}
  // Construct from type-erased child widget list
  Widget(WidgetList &&widgetList) : children_{std::move(widgetList)} {}
  // Construct with parent
  Widget(Widget *par) : parent{par} {
    if (parent)
      parent->adoptChild(this);
  }

  // unsafe:
  // auto addWidgets(ui::Widget& root)
  // {
  //    return ui::Button{&root};   // root is rooted (has parent), so will get
  //    the address on the button in the stack frame of addWidgets
  // }
  //
  // solution #-1: document the issue
  // solution #1: remove constructor from parent: only use add (*)
  // solution #2: make widgets unmoveable
  //    breaks constructors
  // solution #0: all widgets on the heap...
  //    probably the only safe solution
  //    child list = list of optional_ptr<WidgetImpl>
  //    WidgetImpl -> widget (?)
  // Widget = wrapper class to unique_ptr
  // Goal: avoid two construction methods: free functions that return uptr, and
  // constructors
  //  => construct widgets using constructors
  //  => issue: subclassing? essentially two class hierarchies
  //
  // solution #3: UI builder: returns unique_ptrs for all widgets
  //  => No: cannot use constructors

  Widget(Widget &&) = default;
  Widget &operator=(Widget &&) = default;
  Widget(const Widget &) = delete;
  Widget &operator=(const Widget &) = delete;

  virtual ~Widget() {
    // if parented, orphan
    // NOTE: if this widget is owned by another widget,
    // then parent will be set to zero before entering this destructor
    if (parent)
      parent->orphanChild(this);
    // orphan all children
    for (auto &&c : children_.list_) {
      c->parent = nullptr;
    }
  }

  auto &getChildren() { return children_.list_; }

  template <typename T> void add(T &&widget) {
    adoptChild(children_.add(std::forward<T>(widget)));
  }

  // add stuff
  template <typename... Content> void addMany(Content &&... contents) {
    auto dummy = {0, (add(std::forward<Content>(contents)), 0)...};
  }

  virtual void render() {}

protected:
  WidgetList children_;
  Widget *parent{nullptr};

private:
  // Should fail for owned widgets
  void setParent(Widget *newParent) {
    if (parent != nullptr) {
      throw std::logic_error("Widget already has a parent");
    }
    parent = newParent;
    // the parent has been set, which means we are rooted somewhere:
    // we can update the parent field of child elements
    for (auto &c : children_.list_)
      c->setParent(this);
  }

  void adoptChild(Widget *w) {
    if (parent)
      w->setParent(this);
  }

  // might make 'child' invalid if it is owned
  void orphanChild(Widget *child) {
    child->parent = nullptr;
    // TODO could do this lazily
    children_.list_.erase(
        std::remove_if(
            children_.list_.begin(), children_.list_.end(),
            [child](auto &&c) { return !c.owns() && (c.get() == child); }),
        children_.list_.end());
  }
};

struct Separator : public Widget 
{
	void render() override {
		ImGui::Separator();
	}
};

class Menu : public Widget {
public:
  template <typename... Items>
  Menu(const char *text, Items &&... items)
	  : Widget{ WidgetList{std::forward<Items>(items)...} }, text_{ text } {}

  void render() override {
	  if (ImGui::BeginMenu(text_.c_str())) {
		  for (auto& c : children_.list_) {
			  c->render();
		  }
		  ImGui::EndMenu();
	  }
  }

private:
	std::string text_;
};

class MenuBar : public Widget {
public:
  using Widget::Widget; 
  
  void render() override {
	  if (ImGui::BeginMainMenuBar()) {
		  for (auto& c : children_.list_) {
			  c->render();
		  }
		  ImGui::EndMainMenuBar();
	  }
  }
};

class MenuItem : public Widget {
public:
	MenuItem(const char *text, const char *accel) : text_{ text }, accel_{ accel } {}
	
	void render() override {
		ImGui::MenuItem(text_.c_str(), accel_.c_str());
	}

private:
	std::string text_;
	std::string accel_;
};

class Button : public Widget {
public:
	Button(const char* text) : text_{ text }
	{}

private:
	std::string text_;
};

}
}
