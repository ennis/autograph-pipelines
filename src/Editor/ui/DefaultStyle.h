#include "Widget.h"

namespace ag {
	namespace ui {

		//=========================================
		// Sliders
		int hitTestSlider(vec2 pos, ivec2 size, float value, float minVal,
			float maxVal, int nbDivs, float& newVal);
		void getSliderKnobRect(
			ivec2 size, float value, float minVal,
			float maxVal, int nbDivs,
			Rect2D& knobRect, float& knobSnapWidth);
		void drawSlider(
			SkCanvas& cv,
			Widget::VisualState state,
			ivec2 size, float value, float minVal,
			float maxVal, int nbDivs);

		//=========================================
		// Buttons
		void drawButton(SkCanvas &cv, Widget::VisualState s, ivec2 size);

		void drawPanelBackground(SkCanvas &cv, ivec2 size);
	}
}