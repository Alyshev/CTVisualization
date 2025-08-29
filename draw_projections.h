#pragma once

#include <vector>
#include "structs.h"
#include "drawing_functions.h"

int draw_projection_top(values_by_draw& values_draw, const Vector3i& size_CP, const std::vector<uint16_t>& cube_pixels, const values_by_materials* cur_mat);
int draw_projection_right(values_by_draw& values_draw, Vector3i size_CP, std::vector<uint16_t>& cube_pixels, values_by_materials* cur_mat);
int draw_projection_front(values_by_draw& values_draw, Vector3i size_CP, std::vector<uint16_t>& cube_pixels, values_by_materials* cur_mat);