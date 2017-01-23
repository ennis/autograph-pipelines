#pragma once
// This has to be the weirdest code i have ever written
#include <algorithm>
#include <autograph/Types.h>
#include <autograph/engine/Application.h>
#include <autograph/support/Optional.h>
#include <functional>
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

  auto add(OptionalPtr ptr) -> Widget *;
  // add child and take ownership
  auto add(std::unique_ptr<Widget> w) -> Widget *;
  // add reference to heap-allocated widget
  auto add(std::unique_ptr<Widget> &w) -> Widget *;
  // add reference to widget on stack or heap
  // w should not move afterwards
  auto add(Widget &w) -> Widget *;

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
  Widget(Widget *par);

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

  virtual ~Widget();

  auto &getChildren() { return children_.list_; }

  template <typename T> void add(T &&widget) {
    adoptChild(children_.add(std::forward<T>(widget)));
  }

  // add stuff
  template <typename... Content> void addMany(Content &&... contents) {
    auto dummy = {0, (add(std::forward<Content>(contents)), 0)...};
  }

  virtual void render();

protected:
  WidgetList children_;
  Widget *parent{nullptr};

private:
  void setParent(Widget *newParent);
  void adoptChild(Widget *w);
  void orphanChild(Widget *child);
};

struct Separator : public Widget {
  void render() override;
};

class Menu : public Widget {
public:
  template <typename... Items>
  Menu(const char *text, Items &&... items)
      : Widget{WidgetList{std::forward<Items>(items)...}}, text_{text} {}

  void render() override;

private:
  std::string text_;
};

class MenuBar : public Widget {
public:
  using Widget::Widget;
  void render() override;
};

class MenuItem : public Widget {
public:
  MenuItem(const char *text, const char *accel) : text_{text}, accel_{accel} {}
  void render() override;

private:
  std::string text_;
  std::string accel_;
};

class Button : public Widget {
public:
  Button(const char *text) : text_{text} {}

private:
  std::string text_;
};

template <typename T> struct is_callable {
private:
  typedef char (&yes)[1];
  typedef char (&no)[2];

  struct Fallback {
    void operator()();
  };
  struct Derived : T, Fallback {};

  template <typename U, U> struct Check;

  template <typename> static yes test(...);

  template <typename C>
  static no test(Check<void (Fallback::*)(), &C::operator()> *);

public:
  static const bool value = sizeof(test<Derived>(0)) == sizeof(yes);
};

// List model
template <typename T> class Model {
public:
  using ClosureType = std::function<T(ag::optional<T>)>;
  // default
  Model() : closure_{[](auto) { return T{}; }} {}

  // construct from pointer to data
  Model(T *data)
      : closure_{[data](optional<T> v) mutable {
          if (v)
            *data = std::move(*v);
          return *data;
        }} {}

  // construct from closure
  template <typename U,
            typename = std::enable_if_t<
                std::is_constructible<ClosureType, U &&>::value>>
  Model(U closure) : closure_{std::move(closure)} {}

  // construct from constant
  Model(const T &constant) : closure_{[constant](auto) { return constant; }} {}

  T get() { return closure_(nullopt); }

  void set(T t) { closure_(t); }

private:
  ClosureType closure_;
};

// List model
template <typename T> class ListModel {
public:
  using ClosureType = std::function<optional<T>(int, optional<T>)>;

  // default
  ListModel() : closure_{[](auto, auto) { return T{}; }} {}

  // construct from slice of data
  ListModel(span<T> data)
      : closure_{[data](int row, optional<T> v) mutable -> optional<T> {
          if (row >= data.size())
            return nullopt;
          if (v)
            data[row] = std::move(*v);
          return data[row];
        }} {}

  // construct from array
  template <int N>
  ListModel(T (&data)[N])
      : closure_{[&data, N = N](int row, optional<T> v) mutable->optional<T>{
            if (row >= N) return nullopt;
  if (v)
    data[row] = std::move(*v);
  return data[row];
}
}
{}

// construct from const array
template <int N>
ListModel(const T (&data)[N])
    : closure_{[&data, N = N](int row, optional<T> v)
                   ->optional<T>{if (row >= N) return nullopt;
return data[row];
}
}
{}

auto get(int row) { return closure_(row, nullopt); }

void set(int row, T t) { closure_(row, t); }

private:
ClosureType closure_;
}
;

class StaticText : public Widget {
public:
  StaticText() {}
  StaticText(const char *text) : text_{text} {}
  StaticText(std::string text) : text_{std::move(text)} {}
  void render() override;

private:
  std::string text_;
};

class Text : public Widget {
public:
  Text() {}

  Text(Model<const char*> text) : text_{std::move(text)} {}

  void render() override;

private:
  Model<const char*> text_;
};

class TextEdit : public Widget {
public:
  TextEdit() {}
  TextEdit(Model<std::string> text) : text_{std::move(text)} {}
  void render() override;

private:
  Model<std::string> text_;
};

class SliderFloat : public Widget {
public:
  SliderFloat(const char *label, Model<float> value, float initMinValue = 0.0f,
              float initMaxValue = 1.0f)
      : minValue{initMinValue}, maxValue{initMaxValue}, label_{label},
        value_{std::move(value)} {}

  void render() override;

  float minValue = 0.0f;
  float maxValue = 1.0f;

private:
  std::string label_;
  Model<float> value_;
};

class CollapsingHeader : public Widget {
public:
  template <typename... Items>
  CollapsingHeader(const char *label, Items &&... items)
      : Widget{WidgetList{std::forward<Items>(items)...}}, label_{label} {}

  void render() override;

private:
  std::string label_;
  bool collapsed_{true};
};

class ColorPicker : public Widget {
public:
  ColorPicker(Model<vec4> value) : value_{std::move(value)} {}
  void render() override;

private:
  Model<vec4> value_;
};

class ColorPickerAlpha : public Widget {
public:
  ColorPickerAlpha(Model<vec4> value) : value_{std::move(value)} {}
  void render() override;

private:
  Model<vec4> value_;
};

class DockArea : public Widget {
public:
  template <typename... Items>
  DockArea(const char *label, Items &&... items)
      : Widget{WidgetList{std::forward<Items>(items)...}}, label_{label} {}

  void render() override;

private:
  std::string label_;
};

class DockPanel : public Widget {
public:
  template <typename... Items>
  DockPanel(const char *label, Items &&... items)
      : Widget{WidgetList{std::forward<Items>(items)...}}, label_{label} {}

  void render() override;

private:
  std::string label_;
};

class Checkbox : public Widget {
public:
  Checkbox(const char *label, Model<bool> value)
      : label_{label}, value_{std::move(value)} {}

  void render() override;

private:
  std::string label_;
  Model<bool> value_;
};

class ComboBox : public Widget {
public:
  ComboBox(const char *label, ListModel<const char*> values)
      : label_{label}, values_{std::move(values)} {}

  void render() override;
  int selected() const { return selected_; }

private:
  int selected_{-1};
  std::string label_;
  ListModel<const char*> values_;
};

////////////////////////////////////////////////////////
// Graph model
/*class GraphModel
{
public:
	struct Edge {
		int src;
		int srcOutput;
		int dst;
		int dstInput;
	};

	virtual int nodeCount();
	virtual int edgeCount();
	virtual Edge edge(int edgeIndex);
	// Called ONCE per node
	virtual NodeWidget node(int nodeIndex);

private:
};

class NodeGraph : public Widget
{
public:
private:
};

class NodeWidget : public Widget
{
};*/



}
}
