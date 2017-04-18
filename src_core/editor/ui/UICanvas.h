#pragma once
#include "DefaultStyle.h"
#include "Widget.h"
#include <SkPaint.h>
#include <SkCanvas.h>

#include <autograph/support/Debug.h>

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
  
  virtual bool onPointerUp(vec2 pos) override  {
	  Widget::onPointerUp(pos);
	  onClick();
	  return true;
  }

  virtual bool onPointerEnter(vec2 pos) override {
	  Widget::onPointerEnter(pos);
	  return true;
  }

  virtual bool onPointerExit(vec2 pos) override {
	  Widget::onPointerExit(pos);
	  return true;
  }

  Observable<> onClick;

protected:
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

/**
*
*/
class Slider : public Widget
{
public:
	Slider() : label_{"dummy"} 
	{
		//label_.
	}

	virtual void render(SkCanvas& canvas) override {
		drawSlider(canvas, visualState, calculatedTransform.size, value_, minVal_, maxVal_, 0);
		label_.text = std::to_string(value_);
		label_.textSize = calculatedTransform.size.y / 2.0f;
		label_.render(canvas);
	}

	virtual bool onPointerDown(vec2 pos) override {
		auto locPos = calculatedTransform.worldToLocal(pos);
		float newVal;
		int hitTestResult = hitTestSlider(locPos, calculatedTransform.size, value_, minVal_, maxVal_, 0, newVal);
		AG_DEBUG("Slider::onPointerDown pos {},{} locPos {},{} hitTest {} newVal {}", pos.x, pos.y, locPos.x, locPos.y, hitTestResult, newVal);
		if (hitTestResult)
			value_ = newVal;
		return true;
	}

	void setMinValue(float minVal) {
		minVal_ = minVal;
	}
	void setMaxValue(float maxVal) {
		maxVal_ = maxVal;
	}

	Observable<float> value;

private:
	float minVal_{ 0.0f };
	float maxVal_{ 1.0f };
	float value_{ 0.0f };
	Text label_;
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