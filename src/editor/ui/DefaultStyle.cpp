#include "DefaultStyle.h"
#include <SkBlurMaskFilter.h>
#include <SkCanvas.h>
#include <SkPaint.h>

namespace ag {
	namespace ui {

		static const SkColor darkPrimaryColor = 0xFF303F9F;
		static const SkColor defaultPrimaryColor = 0xFF3F51B5;
		static const SkColor lightPrimaryColor = 0xFFC5CAE9;
		static const SkColor textColor = 0xFFFFFFFF;
		static const SkColor accentColor = 0xFF7C4DFF;
		static const SkColor primaryTextColor = 0xFF212121;
		static const SkColor secondaryTextColor = 0xFF757575;
		static const SkColor dividerColor = 0xFFBDBDBD;

		static const SkColor sliderBackgroundColor = darkPrimaryColor;
		static const SkColor sliderKnobColor = defaultPrimaryColor;

		static const SkColor panelBgColor = defaultPrimaryColor;
		static const SkColor labelColor = SkColorSetRGB(255, 255, 255);
		// Button released
		static const SkColor buttonReleasedFillColor = lightPrimaryColor;
		static const SkColor buttonReleasedStrokeColor = SkColorSetRGB(0, 180, 0);
		// Button hover
		static const SkColor buttonHoveredFillColor = defaultPrimaryColor;
		static const SkColor buttonHoveredStrokeColor = SkColorSetRGB(0, 180, 0);
		// Button pressed
		static const SkColor buttonPressedFillColor = darkPrimaryColor;
		static const SkColor buttonPressedStrokeColor = SkColorSetRGB(0, 180, 0);

		static const float roundRectRadius = 3.0f;

		SkPaint getDefaultPaint() {
			SkPaint paint;
			paint.setAntiAlias(true);
			paint.setSubpixelText(true);
			return paint;
		}

		//===========================================
		// Sliders
		

		// Determine slider knob width and position
		void getSliderKnobRect(
			ivec2 size, float value, float minVal, 
			float maxVal, int nbDivs, 
			Rect2D& knobRect, float& knobSnapWidth)
		{
			constexpr float minSliderKnobSize = 10.0f;
			const float fx = (float)size.x;
			const float range = maxVal - minVal;
			const float knobWidth = (nbDivs != 0) ? std::max(minSliderKnobSize, fx / nbDivs) : minSliderKnobSize;
			knobSnapWidth = (nbDivs != 0) ? knobWidth : 1.0f;
			knobRect.position.x = ((glm::clamp(value, minVal, maxVal) - minVal) / range) * (fx - knobWidth);
			knobRect.size.x = knobWidth;
			knobRect.position.y = 0.0f;
			knobRect.size.y = (float)size.y;
		}

		int hitTestSlider(vec2 locPos, ivec2 size, float value, float minVal,
			float maxVal, int nbDivs, float& newVal)
		{
			Rect2D rect{ {0.0f, 0.0f}, {(float)size.x, (float)size.y} };
			Rect2D knob;
			float knobSnapWidth;
			getSliderKnobRect(size, value, minVal, maxVal, nbDivs, knob, knobSnapWidth);
			newVal = (locPos.x / size.x) * (maxVal - minVal) + minVal;
			if (knob.inside(locPos)) {
				return 2;	// clicked knob 
			}
			if (rect.inside(locPos)) {
				return 1;	// clicked rect
			}
			// clicked nothing
			return 0;
		}
	
		void drawSlider(
			SkCanvas& cv, 
			Widget::VisualState state, 
			ivec2 size, float value, float minVal,
			float maxVal, int nbDivs)
		{
			cv.save();
			Rect2D knob;
			float knobSnapWidth;
			getSliderKnobRect(size, value, minVal, maxVal, nbDivs, knob, knobSnapWidth);
			SkRect rect = SkRect::MakeLTRB(0.0f, 0.0f, size.x, size.y);
			SkRect knobRect = SkRect::MakeLTRB(knob.position.x, knob.position.y, knob.position.x+knob.size.x, knob.size.y);
			SkPaint paint = getDefaultPaint();
			paint.setStyle(SkPaint::kFill_Style);
			// draw slider background
			paint.setColor(sliderBackgroundColor);
			cv.drawRect(rect, paint);
			paint.setColor(sliderKnobColor);
			cv.drawRect(knobRect, paint);
			cv.restore();
		}

		//===========================================
		// Buttons
		void drawButton(SkCanvas &cv, Widget::VisualState state, ivec2 size) {
			cv.save();
			SkRect rect = SkRect::MakeLTRB(0.0f, 0.0f, size.x, size.y);
			SkRect rectDrop = SkRect::MakeLTRB(2.0f, 2.0f, size.x, size.y);
			SkPaint paint = getDefaultPaint();
			paint.setStyle(SkPaint::kFill_Style);
			switch (state) {
			case Widget::VisualState::Focused:
				paint.setColor(buttonHoveredFillColor);
				break;
			case Widget::VisualState::Active:
				paint.setColor(buttonPressedFillColor);
				break;
			case Widget::VisualState::Default:
				paint.setColor(buttonReleasedFillColor);
				break;
			}
			SkPaint blur(paint);
			blur.setColor(0xCC000000);
			blur.setMaskFilter(SkBlurMaskFilter::Make(
				kNormal_SkBlurStyle, SkBlurMaskFilter::ConvertRadiusToSigma(5.0f), 0));

			cv.drawRoundRect(rect, roundRectRadius, roundRectRadius, blur);
			cv.drawRoundRect(rect, roundRectRadius, roundRectRadius, paint);
			cv.restore();
		}

		void drawPanelBackground(SkCanvas &cv, ivec2 size)
		{
			SkPaint paint;
			paint.setStyle(SkPaint::kFill_Style);
			paint.setColor(lightPrimaryColor);
			SkRect rect = SkRect::MakeLTRB(0.0f, 0.0f, size.x, size.y);
			cv.drawRect(rect, paint);
		}

	}
}
