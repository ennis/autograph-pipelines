#pragma once
#include "UICanvas.h"

namespace ag {
namespace ui {

struct VerticalLayout : public Widget::Layout 
{
  float spacing {2.0f};
  float paddingLeft {0.0f};
  float paddingRight {0.0f};
  float paddingTop {0.0f};
  float paddingBottom {0.0f};

  void measureContents(Widget& w) override {
    float height = paddingTop + paddingBottom + w.getChildren().size() * spacing;
    float width = 0.0f;
    for (auto &&c : w.getChildren()) {
        width = std::max(width, w.preferredSize.x);
        height += w.preferredSize.y;
        //height += 10.0f;
    }
	  w.preferredSize.x = width;
	  w.preferredSize.y = height;
  }

  void layoutContents(Widget& w) override {
	  auto numChildren = w.getChildren().size();
	  float div = 1.0f / static_cast<float>(numChildren);
	  int i = 0;
	  for (auto &&c : w.getChildren()) {
		  auto& tr = c->rectTransform;
		  float h = i * div;
		  // override user values
		  // fix anchor
		  tr.anchorA = glm::vec2{ 0.0f, h };
		  tr.anchorB = glm::vec2{ 1.0f, h + div };
		  tr.offsetA = glm::vec2{ paddingLeft, 0.0f };
		  tr.offsetB = glm::vec2{ -paddingRight, -spacing };
		  ++i;
	  }
  }
};

struct GridLayout : public Widget::Layout {
	GridLayout() = default;
	GridLayout(int num_rows_, int num_columns_,
		float horizontal_spacing_ = 2.0f,
		float vertical_spacing_ = 2.0f)
		: horizontalSpacing{ horizontal_spacing_ },
		verticalSpacing{ vertical_spacing_ }, numRows{ num_rows_ },
		numColumns{ num_columns_ } {}

	float horizontalSpacing{ 2.0f };
	float verticalSpacing{ 2.0f };
	int numRows{ 5 };
	int numColumns{ 5 };

	void measureContents(Widget& w) override {}

	void layoutContents(Widget& w) override {
		float xdiv = 1.0f / static_cast<float>(numColumns);
		float ydiv = 1.0f / static_cast<float>(numRows);
		int i = 0, j = 0;
		for (auto &&c : w.getChildren()) {
			auto& tr = c->rectTransform;
			float x = j * xdiv;
			float y = i * ydiv;
			tr.anchorA = glm::vec2{ x, y };
			tr.anchorB = glm::vec2{ x + xdiv, y + ydiv };
			tr.offsetA = glm::vec2{ 0.0f, 0.0f };
			tr.offsetB = glm::vec2{ -horizontalSpacing, -verticalSpacing };
			++j;
			if (j >= numColumns) {
				j = 0;
				++i;
			}
		}
	}
};

}}
