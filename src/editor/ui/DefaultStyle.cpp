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