#include "Widget.h"
#include <imgui.h>
#include <autograph/support/Debug.h>

namespace ag {
namespace ui {

auto WidgetList::add(OptionalPtr ptr) -> Widget * {
  auto p = ptr.get();
  list_.push_back(std::move(ptr));
  return p;
}

// add child and take ownership
auto WidgetList::add(std::unique_ptr<Widget> w) -> Widget * {
  return add(OptionalPtr{std::move(w)});
}

// add reference to heap-allocated widget
auto WidgetList::add(std::unique_ptr<Widget> &w) -> Widget * {
  return add(OptionalPtr{w.get()});
}

// add reference to widget on stack or heap
// w should not move afterwards
auto WidgetList::add(Widget &w) -> Widget * { return add(OptionalPtr{&w}); }

// Construct with parent
Widget::Widget(Widget *par) : parent{par} {
  if (parent)
    parent->adoptChild(this);
}

Widget::~Widget() {
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

void Widget::render() 
{
	for (auto &c : children_.list_) {
		c->render();
	}
}

// Should fail for owned widgets
void Widget::setParent(Widget *newParent) {
  if (parent != nullptr) {
    throw std::logic_error("Widget already has a parent");
  }
  parent = newParent;
  // the parent has been set, which means we are rooted somewhere:
  // we can update the parent field of child elements
  for (auto &c : children_.list_)
    c->setParent(this);
}

void Widget::adoptChild(Widget *w) {
  if (parent)
    w->setParent(this);
}

void Widget::orphanChild(Widget *child) {
  child->parent = nullptr;
  // TODO could do this lazily
  children_.list_.erase(std::remove_if(children_.list_.begin(),
                                       children_.list_.end(),
                                       [child](auto &&c) {
                                         return !c.owns() && (c.get() == child);
                                       }),
                        children_.list_.end());
}

void Separator::render() { ImGui::Separator(); }

void Menu::render() {
  if (ImGui::BeginMenu(text_.c_str())) {
    for (auto &c : children_.list_) {
      c->render();
    }
    ImGui::EndMenu();
  }
}

void MenuBar::render() {
  if (ImGui::BeginMainMenuBar()) {
    for (auto &c : children_.list_) {
      c->render();
    }
    ImGui::EndMainMenuBar();
  }
}

void MenuItem::render() { ImGui::MenuItem(text_.c_str(), accel_.c_str()); }

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
    for (auto &c : children_.list_) {
      c->render();
    }
  }
}

void ColorPicker::render() 
{
	vec4 v = value_.get();
	ImGui::ColorPicker(&v[0], false);
    value_.set(v);
}

void ColorPickerAlpha::render()
{
	vec4 v = value_.get();
	ImGui::ColorPicker(&v[0], true);
	value_.set(v);
}

void DockArea::render()
{
	Widget::render();
}

void DockPanel::render()
{
	if (ImGui::BeginDock(label_.c_str())) {
		Widget::render();
		ImGui::EndDock();
	}
}

void Checkbox::render()
{
	bool v = value_.get();
	ImGui::Checkbox(label_.c_str(), &v);
	value_.set(v);
}

void ComboBox::render()
{
	// count number of values from model
	// this is ridiculous
	int count = 0;
	while (values_.get(count)) { count++; }
	ImGui::Combo(label_.c_str(), &selected_, [](void* data, int idx, const char** out_text) {
		auto this_ = static_cast<ComboBox*>(data);
		auto v = this_->values_.get(idx);
		if (v) {
			*out_text = *v;
			return true;
		}
		else {
			return false;
		}
	}, this, count);
}

}
}