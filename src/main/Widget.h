#pragma once
// This has to be the weirdest code i have ever written
#include <algorithm>
#include <autograph/Types.h>
#include <autograph/engine/Application.h>
#include <autograph/engine/Meta.h>
#include <autograph/engine/Observable.h>
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

/**
 * @brief A widget in the visual tree
 * @details [long description]
 *
 */
class Widget {
public:
  Widget() {}
  // Construct with parent
  Widget(Widget *par);
  // Disable move and copy constructors
  Widget(const Widget &) = delete;
  Widget &operator=(const Widget &) = delete;
  Widget(Widget &&) = delete;
  Widget &operator=(Widget &&) = delete;

  virtual ~Widget();

  // add and set parents
  auto add(OptionalPtr ptr) -> Widget *;
  // add child and take ownership
  auto add(std::unique_ptr<Widget> w) -> Widget *;
  // add reference to heap-allocated widget
  auto add(std::unique_ptr<Widget> &w) -> Widget *;
  // add reference to widget on stack or heap
  // w should not move afterwards
  auto add(Widget *w) -> Widget *;

  auto &getChildren() { return children_; }

  // add stuff
  template <typename... Widgets> void addMany(Widgets &&... ws) {
    auto dummy = {0, (add(std::forward<Widgets>(ws)), 0)...};
  }

  virtual void render();

protected:
  std::vector<OptionalPtr> children_;
  Widget *parent_{nullptr};

private:
  void setParent(Widget *newParent);
  void orphanChild(Widget *child);
};

struct Separator : public Widget {
  using Widget::Widget;
  void render() override;
};

class Menu : public Widget {
public:
  template <typename... Items>
  Menu(Widget *parent, const char *text) : Widget{parent}, text_{text} {}

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
  MenuItem(Widget *parent, const char *text, const char *accel)
      : Widget{parent}, text_{text}, accel_{accel} {}

  template <typename Fn>
  MenuItem(Widget *parent, const char *text, const char *accel, Fn &&clicked_)
      : Widget{parent}, text_{text}, accel_{accel} {
    clicked.subscribe(std::forward<Fn>(clicked_));
  }

  void render() override;

  Observable<> clicked;

private:
  std::string text_;
  std::string accel_;
};

class Button : public Widget {
public:
  Button(Widget *parent, const char *text) : Widget{parent}, text_{text} {}

  template <typename Fn> Button(const char *text, Fn &&clicked_) : text_{text} {
    clicked.subscribe(std::forward<Fn>(clicked_));
  }
  void render() override;

  Observable<> clicked;

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
/*template <typename T> class ListModel {
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
        } } {}

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


// construct from closure
template <typename U,
        typename = std::enable_if_t<std::is_constructible<ClosureType, U
&&>::value>>
        ListModel(U closure) : closure_{ std::move(closure) } {}

auto get(int row) { return closure_(row, nullopt); }

void set(int row, T t) { closure_(row, t); }

private:
ClosureType closure_;
}
;*/

class StaticText : public Widget {
public:
  StaticText() {}
  StaticText(Widget *parent, const char *text) : Widget{parent}, text_{text} {}
  StaticText(Widget *parent, std::string text)
      : Widget{parent}, text_{std::move(text)} {}
  void render() override;

private:
  std::string text_;
};

class Text : public Widget {
public:
  Text() {}
  Text(Widget *parent, Model<const char *> text)
      : Widget{parent}, text_{std::move(text)} {}

  void setText(std::string s)
  {
	  text_ = [s = std::move(s)](auto){ return s.c_str(); };
  }

  void setText(const char* text)
  {
	  text_ = [str = std::string{ text }](auto){ return str.c_str(); };
  }

  void render() override;

private:
  Model<const char *> text_;
};

class TextEdit : public Widget {
public:
  TextEdit() {}
  TextEdit(Widget *parent, Model<std::string> text)
      : Widget{parent}, text_{std::move(text)} {}

  void setModel(Model<std::string> text)
  {
	  text_ = std::move(text);
  }

  void render() override;

private:
  Model<std::string> text_;
};

class SliderFloat : public Widget {
public:
  SliderFloat(Widget *parent, const char *label, Model<float> value,
              float initMinValue = 0.0f, float initMaxValue = 1.0f)
      : Widget{parent}, minValue{initMinValue}, maxValue{initMaxValue},
        label_{label}, value_{std::move(value)} {}

  void render() override;

  float minValue = 0.0f;
  float maxValue = 1.0f;

private:
  std::string label_;
  Model<float> value_;
};

class CollapsingHeader : public Widget {
public:
  CollapsingHeader(Widget *parent, const char *label)
      : Widget{parent}, label_{label} {}

  void render() override;

private:
  std::string label_;
  bool collapsed_{true};
};

class ColorPicker : public Widget {
public:
  ColorPicker(Widget *parent, Model<vec4> value) : Widget{ parent }, value_{std::move(value)} {}
  void render() override;

private:
  Model<vec4> value_;
};

class ColorPickerAlpha : public Widget {
public:
  ColorPickerAlpha(Widget *parent, Model<vec4> value)
      : Widget{ parent }, value_{std::move(value)} {}
  void render() override;

private:
  Model<vec4> value_;
};

class DockArea : public Widget {
public:
  DockArea(Widget *parent, const char *label) : Widget{parent}, label_{label} {}

  void render() override;

private:
  std::string label_;
};

class DockPanel : public Widget {
public:
  DockPanel(Widget *parent, const char *label)
      : Widget{parent}, label_{label} {}

  void render() override;

private:
  std::string label_;
};

class Checkbox : public Widget {
public:
  Checkbox(Widget *parent, const char *label, Model<bool> value)
      : Widget{parent}, label_{label}, value_{std::move(value)} {}

  void render() override;

private:
  std::string label_;
  Model<bool> value_;
};

//
class ListSelectionModel {
public:
  virtual const char *elementName(int index) = 0;
  virtual bool isElementSelected(int index) = 0;
  virtual void setSelection(int index, bool v) = 0;
  virtual int elementCount() = 0;

  Observable<int> selectionChanged;
};

/*class StringListSelectionModel : public ListSelectionModel {
public:
  StringListSelectionModel(span<const char *const> stringList)
      : stringList_{stringList} {
    selection_.resize(stringList.size());
  }

  const char *elementName(int index) override { return stringList_[index]; }
  bool isElementSelected(int index) override { return selection_[index]; }
  void setSelection(int index, bool v) override { selection_[index] = v; }
  int elementCount() { return stringList_.size(); }
  span<const int> getSelection() const { return span<const int>{selection_}; }

protected:
  std::vector<int> selection_;
  span<const char *const> stringList_;
};

class ListBox : public Widget {
public:
  ListBox(Widget *parent, const char *label, ListSelectionModel *model)
      : Widget{parent}, label_{label}, model_{model} {}

  void render() override;

private:
  std::string label_;
  ListSelectionModel *model_;
};

// Combo box model
class ComboBoxModel {
public:
  virtual const char *elementName(int index) = 0;
  virtual int selectedElement() = 0;
  virtual int setSelectedElement(int index) = 0;
  virtual int elementCount() = 0;

  Observable<int> selectionChanged;
};

// Combo box model: list of strings
class StringListComboBoxModel : public ComboBoxModel {
public:
  StringListComboBoxModel(span<const char *const> stringList)
      : stringList_{stringList} {}

  const char *elementName(int index) override {
    if (index < stringList_.size())
      return stringList_[index];
    return nullptr;
  }

  int selectedElement() override { return selected_; }

  int setSelectedElement(int index) override {
    if (index < stringList_.size())
      selected_ = index;
    return selected_;
  }

  int elementCount() override { return stringList_.size(); }

protected:
  int selected_;
  span<const char *const> stringList_;
};

// Combo box model for an enum type (uses meta information)
template <typename T> class EnumComboBoxModel : public ComboBoxModel {
  static_assert(std::is_enum<T>::value, "T must be an enumeration type");

public:
  EnumComboBoxModel() { enumData_ = meta::typeOf<T>()->template as<meta::Enum>(); }

  const char *elementName(int index) override {
    if (index < elementCount())
      return enumData_->enumerators[index].name;
    return nullptr;
  }

  int selectedElement() override { return selected_; }

  int setSelectedElement(int index) override {
    if (index < elementCount())
      selected_ = index;
    return selected_;
  }

  T selectedEnumValue() const {
    return static_cast<T>(enumData_->enumerators[selected_].value);
  }

  int elementCount() override { return enumData_->enumerators.size(); }

private:
  int selected_;
  const meta::Enum *enumData_;
};

class ComboBox : public Widget {
public:
  ComboBox(Widget *parent, const char *label, ComboBoxModel *model)
      : Widget{parent}, label_{label}, model_{model} {}

  void render() override;
  int selected() const { return model_->selectedElement(); }

private:
  std::string label_;
  ComboBoxModel *model_;
};*/

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
