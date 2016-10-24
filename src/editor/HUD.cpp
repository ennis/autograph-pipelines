#include <SkCanvas.h>
#include <SkPaint.h>
#include <autograph/RectTransform.h>
//#include <editor/Scene.hpp>
//#include <editor/ui/Button.hpp>
//#include <editor/ui/HUD.hpp>
//#include <editor/ui/Layout.hpp>
//#include <editor/ui/Selectable.hpp>
//#include <editor/ui/Text.hpp>

namespace ui {
/*void HUDUpdateTransforms(Scene &s, uint64_t eid, const glm::mat3 &parentTr,
                         glm::vec2 parentSize) {
  // 2 read components: RectTransform, LayoutController
  if (auto rect_tr = s.getComponent<RectTransform>(eid)) {
    rect_tr->updateTransform(parentTr, parentSize);
    if (auto layoutController = s.getComponent<LayoutController>(eid)) {
      if (layoutController)
        layoutController->layoutContents(s, eid, *rect_tr);
    }
    for (auto &&child : rect_tr->children)
      HUDUpdateTransforms(s, child, rect_tr->calcTransform, rect_tr->calcSize);
  }
}

void HUDSetMatrix(HUDRenderer &renderer, const glm::mat3 &matrix) {
  SkMatrix skMatrix;
  auto trMatrix = glm::transpose(matrix);
  skMatrix.set9(&trMatrix[0][0]);
  renderer.canvas->setMatrix(skMatrix);
}

void HUDRenderRecursive(Scene &s, HUDRenderer &renderer, uint64_t eid) {
  // Components: rectTransform, visual
  auto widget = s.getComponent<ui::Widget>(eid);
  auto transform = s.getComponent<RectTransform>(eid);
  if (!transform)
    return;
  HUDSetMatrix(renderer, transform->calcTransform);
  if (widget)
    widget->render(renderer, transform->calcSize);
  for (auto child : transform->children) {
    HUDRenderRecursive(s, renderer, child);
  }
}

void HUDRender(Scene &s, glm::vec2 canvasSize, SkCanvas *canvas,
               uint64_t rootHUDElement) {
  HUDRenderer renderer{canvas};
  HUDUpdateTransforms(s, rootHUDElement, glm::mat3{1.0f}, canvasSize);
  HUDRenderRecursive(s, renderer, rootHUDElement);
}

bool HUDHitTestRecursive(Scene &s, uint64_t eid, input::InputEvent &ev,
                         glm::vec2 p) {
  // 2 read components: RectTransform, Selectable
  auto widget = s.getComponent<Widget>(eid);
  auto tr = s.getComponent<RectTransform>(eid);
  if (!tr)
    return false;
  bool isInside = tr->isPointInside(p);
  bool handled = false;
  // always hit-test children?
  for (auto &&c : tr->children)
    handled |= HUDHitTestRecursive(s, c, ev, p);
  if (widget) {
    switch (ev.type) {
    case input::InputEventType::MouseButton: {
      // Mouse button events are only handled if the children have not handled
      // it
      if (!handled && isInside) {
        auto mbev = ev.as<input::MouseButtonEvent>();
        if (mbev->button == 0) {
          if (mbev->state == input::ButtonState::Pressed) {
            handled |= widget->onPointerDown();
          } else {
            handled |= widget->onPointerUp();
          }
        }
      }
    } break;
    case input::InputEventType::Cursor: {
      // cursor events are handled unconditionnally
      auto cursorev = ev.as<input::CursorEvent>();
      auto state = widget->getState();
      if (!isInside && state == Widget::State::Hovered) {
        //
        widget->onPointerExit();
      } else if (isInside && state == Widget::State::Released) {
        widget->onPointerEnter();
      }

    } break;
    }
  }
  return handled;
}

bool HUDHandleInput(Scene &s, uint64_t rootHUDElement, input::InputEvent &ev) {
  using namespace input;
  glm::ivec2 cursorPos{getCursorPos()};
  glm::vec2 cursorPosF{(float)cursorPos.x, (float)cursorPos.y};
  switch (ev.type) {
  case input::InputEventType::MouseButton:
  case input::InputEventType::Cursor:
    return HUDHitTestRecursive(s, rootHUDElement, ev, cursorPosF);
  default:
    break;
  }
  return false;
}

uint64_t createButton(Scene &s, uint64_t parent, std::string text) {
  auto e = s.createEntity();
  auto rt = s.addComponent<RectTransform>(e.id);
  rt->parent = parent;
  auto b = s.addComponent<Button>(e.id);
  if (s.isValid(parent))
    s.getComponent<RectTransform>(parent)->children.push_back(e);
  auto text_e = s.createEntity();
  s.addComponent<RectTransform>(text_e)->parent = e;
  s.addComponent<Text>(text_e, std::move(text), 40.0f,
                       glm::vec4{0.0f, 0.0f, 0.0f, 1.0f}, true);
  s.getComponent<RectTransform>(e)->children.push_back(text_e);
  return e;
}

uint64_t createPanel(Scene &s, uint64_t parent) {
  auto ent = s.createEntity();
  s.addComponent<RectTransform>(ent)->parent = parent;
  s.addComponent<ui::VerticalLayout>(ent);
  if (s.isValid(parent))
    s.getComponent<RectTransform>(parent)->children.push_back(ent);
  return ent;
}*/
}