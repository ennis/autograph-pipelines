#pragma once
#include "../rect.hpp"
#include "size_policy.hpp"

namespace ui {
enum class anchor { nw, nn, ne, ww, c, ee, sw, ss, se };

rect_2d layout_rect(const rect_2d &area, const rect_2d &elem, size_policy wfill,
                    size_policy hfill, anchor anchor);
}