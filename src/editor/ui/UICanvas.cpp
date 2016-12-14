#pragma once
#include "UICanvas.h"
#include <SkCanvas.h>
#include <SkPaint.h>
#include "DefaultStyle.h"

class SkCanvas;

namespace ag {
namespace ui {

void UICanvas::updateTransforms(Widget& w, const mat3 &parentTr,
                                vec2 parentSize) {
  w.calculatedTransform =
      w.rectTransform.calculateTransform(parentTr, parentSize);
  if (w.layout) {
    w.layout->layoutContents(w);
  }
  for (auto &&c : w.children) {
    updateTransforms(*c, w.calculatedTransform.transform,
                     w.calculatedTransform.size);
  }
}

void skSetMatrix(SkCanvas& canvas, const mat3 &matrix)
{
	SkMatrix skMatrix;
	auto trMatrix = glm::transpose(matrix);
	skMatrix.set9(&trMatrix[0][0]);
	canvas.setMatrix(skMatrix);
}

bool UICanvas::onInput(Widget & rootWidget, InputEvent & ev)
{
	ivec2 cursorPos{ getCursorPosition() };
	vec2 cursorPosF{ (float)cursorPos.x, (float)cursorPos.y };
	switch (ev.type) {
	case InputEventType::MouseButton:
	case InputEventType::Cursor:
		return recursiveHitTest(rootWidget, ev, cursorPosF);
	default:
		break;
	}
	return false;
}

void UICanvas::render(Widget& rootWidget, SkCanvas &canvas, vec2 canvasSize)
{
	updateTransforms(rootWidget, mat3{ 1.0f }, canvasSize);
	recursiveRender(rootWidget, canvas);
}

void UICanvas::recursiveRender(Widget& w, SkCanvas &canvas)
{
	skSetMatrix(canvas, w.calculatedTransform.transform);
	canvas.save();
	w.render(canvas);
	for (auto&& child : w.children) {
		recursiveRender(*child, canvas);
	}
	canvas.restore();
}

bool UICanvas::recursiveHitTest(Widget& w, InputEvent &ev, vec2 pos) {
	bool isInside = w.calculatedTransform.isPointInside(pos);
  bool handled = false;
  // always hit-test children?
  for (auto &&c : w.children)
    handled |= recursiveHitTest(*c, ev, pos);

  switch (ev.type) {
    case InputEventType::MouseButton: {
      // Mouse button events are only handled if the children have not handled
      // it
      if (!handled && isInside) {
        auto mbev = ev.as<MouseButtonEvent>();
        if (mbev->button == 0) {
          if (mbev->state == ButtonState::Pressed) {
            handled |= w.onPointerDown(pos);
          } else {
            handled |= w.onPointerUp(pos);
          }
        }
      }
    } break;
    case InputEventType::Cursor: {
      // cursor events are handled unconditionnally
      auto cursorev = ev.as<CursorEvent>();
      if (!isInside && w.visualState == Widget::VisualState::Focused) {
        w.visualState = Widget::VisualState::Default;
        w.onPointerExit(pos);
      } else if (isInside && w.visualState == Widget::VisualState::Default) {
        w.visualState = Widget::VisualState::Focused;
        w.onPointerEnter(pos);
      }
    } break;
   }
 
  return handled;
}
}
}