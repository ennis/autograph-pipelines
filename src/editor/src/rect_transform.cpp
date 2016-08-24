#include "rect_transform.hpp"
#include <nanovg.h>
#include <imgui.h>
#include <glm/gtx/matrix_transform_2d.hpp>

void rect_transform::debug(NVGcontext *vg) {
	// Local space: draw stuff
	/*glm::vec2 piv_trans = pivot * cached_rect_.size;
	nvgBeginPath(vg);
	nvgRect(vg, 0, 0, cached_rect_.size.x, cached_rect_.size.y);
	nvgStrokeColor(vg, nvgRGBA(0, 0, 0, 255));
	nvgStrokeWidth(vg, 1.0f);
	nvgStroke(vg);

	nvgBeginPath(vg);
	nvgCircle(vg, piv_trans.x, piv_trans.y, 4.0f);
	nvgStrokeColor(vg, nvgRGBA(0, 0, 0, 0));
	nvgFillColor(vg, nvgRGB(255, 0, 0));
	nvgFill(vg);*/
}

void rect_transform::test_gui() {
	ImGui::SliderFloat2("Offset A", &offset_a[0], 0.0f, 6000.0f, "%.1f", 1.3f);
	ImGui::SliderFloat2("Offset B", &offset_b[0], 0.0f, 6000.0f, "%.1f", 1.3f);
	ImGui::SliderFloat2("Anchor A", &anchor_a[0], 0.0f, 1.0f);
	ImGui::SliderFloat2("Anchor B", &anchor_b[0], 0.0f, 1.0f);
	ImGui::SliderFloat2("Pivot", &pivot[0], 0.0f, 1.0f);
	ImGui::SliderFloat("Rotation", &rotation[0], 0.0f, 2.0f * 3.141592f);
	ImGui::SliderFloat2("Scale", &scale[0], 0.01f, 60000.0f, "%.1f", 1.3f);
	ImGui::Separator();
	/*float calc[4];
	calc[0] = cached_rect_.pos.x;
	calc[1] = cached_rect_.pos.y;
	calc[2] = cached_rect_.size.x;
	calc[3] = cached_rect_.size.y;
	ImGui::SliderFloat4("Calculated", calc, 0.0f, 60000.0f);*/
}

void rect_transform::update_transform(const glm::mat3& parent_tr, glm::vec2 parent_size) 
{
	// anchor positions (relative)
	float anchor_top = parent_size.y * anchor_a.y;
	float anchor_bottom = parent_size.y * anchor_b.y;
	float anchor_left = parent_size.x * anchor_a.x;
	float anchor_right = parent_size.x * anchor_b.x;

	// rect corners
	float rect_top = anchor_top + offset_a.y;
	float rect_bottom = anchor_bottom + offset_b.y; // it can be negative
	float rect_left = anchor_left + offset_a.x;
	float rect_right = anchor_right + offset_b.x;

	// ignore scale for now
	calc_size.x = std::round(rect_right - rect_left);
	calc_size.y = std::round(rect_bottom - rect_top);

	auto pos = glm::vec2{ std::round(rect_left) + 0.5f, std::round(rect_top) + 0.5f };
	glm::vec2 piv_trans = pivot * calc_size;
	calc_transform = parent_tr;
	calc_transform = glm::translate(calc_transform, pos + piv_trans);
	calc_transform = glm::rotate(calc_transform, rotation.x);
	calc_transform = glm::translate(calc_transform, -piv_trans);
}