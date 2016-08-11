#pragma once
#include "visual.hpp"
#include <string>
#include <nanovg.h>

namespace ui
{
	enum class font_weight {
		thin, regular, bold
	};

	struct text_visual : public visual
	{
		text_visual() = default;
		text_visual(std::string text_, float font_size_, glm::vec4 color_, bool auto_fit_ = false) :
			text{ text_ },
			font_size{ font_size_ },
			color{ color_ },
			auto_fit{ auto_fit_ }
		{}

		void render(NVGcontext *nvg, glm::vec2 size) override
		{
			nvgTextAlign(nvg, NVG_ALIGN_CENTER | NVG_ALIGN_TOP);
			if (auto_fit) 
				nvgFontSize(nvg, size.y * 0.9f);
			else
				nvgFontSize(nvg, font_size);
			nvgFillColor(nvg, nvgRGBAf(color.r, color.g, color.b, color.a));
			nvgTextBox(nvg, 0.0f, 0.0f, size.x, text.c_str(), nullptr);
		}

		std::string text;
		float font_size;
		glm::vec4 color;
		bool auto_fit = false;
	};
}