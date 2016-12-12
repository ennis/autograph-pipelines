#pragma once
#include "DefaultStyle.h"
#include "Widget.h"
#include <SkPaint.h>
#include <SkCanvas.h>

namespace ag {
namespace ui {

/**
 *
 */
class Panel : public Widget {
public:
  virtual void render(SkCanvas &canvas) override {
    // draw a panel
	  drawPanelBackground(canvas, calculatedTransform.size);
  }

protected:
};

/**
*
*/
class Button : public Widget {
public:
  virtual void render(SkCanvas &canvas) override {
    // draw a button
	  drawButton(canvas, visualState, calculatedTransform.size);
  }
  
  virtual bool onPointerUp() override  {
	  Widget::onPointerUp();
	  onClick();
	  return true;
  }

  virtual bool onPointerEnter() override {
	  Widget::onPointerEnter();
	  return true;
  }

  virtual bool onPointerExit() override {
	  Widget::onPointerExit();
	  return true;
  }

  Observable<> onClick;

protected:
};

/**
 *
 */
class Slider : public Widget
{
public:
	virtual void render(SkCanvas& canvas) override {

	}

	Observable<float> value;

private:
	float value_;
};

/**
*
*/
class Text : public Widget {
public:
  Text(const char *text_) : text{text_} {}

  virtual void render(SkCanvas &canvas) override {
	  SkPaint paint;
	  paint.setStyle(SkPaint::kFill_Style);
	  paint.setColor(SkColorSetRGB(0, 0, 0));
	  paint.setTextSize(textSize);
	  paint.setTextAlign(SkPaint::kLeft_Align);
	  paint.setSubpixelText(true);
	  paint.setLCDRenderText(true);
	  paint.setAntiAlias(true);
	  canvas.drawText(text.c_str(), text.size(), 0.0f, textSize, paint);
  }

  std::string text;
  float textSize{ 12.0f };
  bool autoFit{ false };
};

class Dummy : public Widget {
public:
	virtual void render(SkCanvas &canvas) override {
	}
};

/**
 * @brief UI canvas
 * @details Renders widgets
 *
 */
class UICanvas {
public:
  bool onInput(Widget &rootWidget, InputEvent &ev);
  void render(Widget &rootWidget, SkCanvas &canvas, vec2 canvasSize);

private:
  void updateTransforms(Widget &w, const mat3 &parentTr, vec2 parentSize);
  void recursiveRender(Widget &w, SkCanvas &canvas);
  bool recursiveHitTest(Widget &w, InputEvent &ev, vec2 pos);
};
}
}