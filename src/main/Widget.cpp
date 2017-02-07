#include "Widget.h"
#include <autograph/support/Debug.h>
#include <imgui.h>

namespace ag {
namespace ui {

auto Widget::add(OptionalPtr ptr) -> Widget * {
  auto p = ptr.get();
  children_.push_back(std::move(ptr));
  p->setParent(this);
  return p;
}

// add child and take ownership
auto Widget::add(std::unique_ptr<Widget> w) -> Widget * {
  return add(OptionalPtr{std::move(w)});
}

// add reference to heap-allocated widget
auto Widget::add(std::unique_ptr<Widget> &w) -> Widget * {
  return add(OptionalPtr{w.get()});
}

// add reference to widget on stack or heap
// w should not move afterwards
auto Widget::add(Widget *w) -> Widget * { return add(OptionalPtr{w}); }

// Construct with parent
Widget::Widget(Widget *par) {
  if (par)
    par->add(this); // will set parent_
}

Widget::~Widget() {
  // if parented, orphan
  // NOTE: if this widget is owned by another widget,
  // then parent will be set to zero before entering this destructor
  if (parent_)
    parent_->orphanChild(this);
  // orphan all children
  for (auto &&c : children_) {
    c->parent_ = nullptr;
  }
}

void Widget::render() {
  for (auto &c : children_) {
    c->render();
  }
}

// Should fail for owned widgets
void Widget::setParent(Widget *newParent) {
  if (parent_ != nullptr) {
    throw std::logic_error("Widget already has a parent");
  }
  parent_ = newParent;
}

void Widget::orphanChild(Widget *child) {
  child->parent_ = nullptr;
  // TODO could do this lazily
  children_.erase(std::remove_if(children_.begin(), children_.end(),
                                 [child](auto &&c) {
                                   return !c.owns() && (c.get() == child);
                                 }),
                  children_.end());
}

void Separator::render() { ImGui::Separator(); }

void Menu::render() {
  if (ImGui::BeginMenu(text_.c_str())) {
    for (auto &c : children_) {
      c->render();
    }
    ImGui::EndMenu();
  }
}

void MenuBar::render() {
  if (ImGui::BeginMainMenuBar()) {
    for (auto &c : children_) {
      c->render();
    }
    ImGui::EndMainMenuBar();
  }
}

void MenuItem::render() {
  if (ImGui::MenuItem(text_.c_str(), accel_.c_str()))
    clicked();
}

void StaticText::render() { ImGui::Text("%s", text_.c_str()); }

void Text::render() {
  auto str = text_.get();
  ImGui::Text("%s", str);
}

void TextEdit::render() {
  auto str = text_.get();
  std::vector<char> strvec{str.begin(), str.end()};
  strvec.resize(100);
  ImGui::InputText("", strvec.data(), strvec.size());
  str.assign(strvec.begin(), strvec.end());
  text_.set(str);
}

void SliderFloat::render() {
  float v = value_.get();
  ImGui::SliderFloat(label_.c_str(), &v, minValue, maxValue);
  value_.set(v);
}

void CollapsingHeader::render() {
  if (ImGui::CollapsingHeader(label_.c_str(), &collapsed_)) {
    for (auto &c : children_) {
      c->render();
    }
  }
}

void ColorPicker::render() {
  vec4 v = value_.get();
  ImGui::ColorPicker(&v[0], false);
  value_.set(v);
}

void ColorPickerAlpha::render() {
  vec4 v = value_.get();
  ImGui::ColorPicker(&v[0], true);
  value_.set(v);
}

void DockArea::render() { Widget::render(); }

void DockPanel::render() {
  if (ImGui::BeginDock(label_.c_str())) {
    Widget::render();
    ImGui::EndDock();
  }
}

void Checkbox::render() {
  bool v = value_.get();
  ImGui::Checkbox(label_.c_str(), &v);
  value_.set(v);
}

void ComboBox::render() {
  // count number of values from model
  // this is ridiculous
  int count = model_->elementCount();
  int selected = model_->selectedElement();
  if (ImGui::Combo(label_.c_str(), &selected,
                   [](void *data, int idx, const char **out_text) {
                     auto this_ = static_cast<ComboBox *>(data);
                     auto v = this_->model_->elementName(idx);
                     if (v) {
                       *out_text = v;
                       return true;
                     } else {
                       return false;
                     }
                   },
                   this, count)) {
    model_->setSelectedElement(selected);
    model_->selectionChanged(selected);
  }
}

void ListBox::render() {
  int count = model_->elementCount();
  for (int i = 0; i < count; ++i) {
    ImGui::PushID(i);
    bool selected = model_->isElementSelected(i);
    bool fire = ImGui::Selectable(model_->elementName(i), &selected);
    model_->setSelection(i, selected);
    if (fire)
      model_->selectionChanged(i);
    ImGui::PopID();
  }
}

void Button::render() {
  if (ImGui::Button(text_.c_str())) {
    clicked();
  }
}
}
}