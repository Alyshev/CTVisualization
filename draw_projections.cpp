#include "draw_projections.h"

int draw_projection_top(values_by_draw& values_draw, const Vector3i& size_CP, const std::vector<uint16_t>& cube_pixels, const values_by_materials* cur_mat)
{
    for (int j = 0; j < size_CP.y; ++j) {
        for (int k = 0; k < size_CP.x; ++k) {
            int value = cube_pixels[static_cast<size_t>(cur_mat->shift) * size_CP.y * size_CP.x + static_cast<size_t>(j) * size_CP.x + static_cast<size_t>(k)];
            const float scale = 255.0f / (cur_mat->max_vis_win - (cur_mat->min_vis_win));
            value = static_cast<int>((value - (cur_mat->min_vis_win)) * scale + 0.5f);
            const uint8_t intensity = static_cast<uint8_t>((value < 0.0f) ? 0.0f : (value > 255.0f) ? 0.0f : value);

            draw_pixel(values_draw, k, j, color(intensity, intensity, intensity));
            //int mult = 1;
            //draw_rectangle(k* mult, j* mult, mult, mult, color(col, col, col));
        }
    }
    return 0;
}

int draw_projection_right(values_by_draw& values_draw, Vector3i size_CP, std::vector<uint16_t>& cube_pixels, values_by_materials* cur_mat)
{
    for (int i = 1; i < size_CP.z; ++i) {
        for (int j = 0; j < size_CP.y; ++j) {
            int value = cube_pixels[static_cast<size_t>(i) * size_CP.y * size_CP.x + static_cast<size_t>(j) * size_CP.x + static_cast<size_t>(cur_mat->shift)];
            const float scale = 255.0f / (cur_mat->max_vis_win - (cur_mat->min_vis_win));
            value = static_cast<int>((value - (cur_mat->min_vis_win)) * scale + 0.5f);
            const uint8_t intensity = static_cast<uint8_t>((value < 0.0f) ? 0.0f : (value > 255.0f) ? 0.0f : value);

            draw_pixel(values_draw, i, j, color(intensity, intensity, intensity));
            //int mult = 1;
            //draw_rectangle(i * mult, j * mult, mult, mult, color(col, col, col));
        }
    }
    return 0;
}

int draw_projection_front(values_by_draw& values_draw, Vector3i size_CP, std::vector<uint16_t>& cube_pixels, values_by_materials* cur_mat)
{
    for (int i = 0; i < size_CP.z; ++i) {
        for (int k = 0; k < size_CP.x; ++k) {
            int value = cube_pixels[static_cast<size_t>(i) * size_CP.y * size_CP.x + static_cast<size_t>(cur_mat->shift) * size_CP.x + static_cast<size_t>(k)];
            const float scale = 255.0f / (cur_mat->max_vis_win - (cur_mat->min_vis_win));
            value = static_cast<int>((value - (cur_mat->min_vis_win)) * scale + 0.5f);
            const uint8_t intensity = static_cast<uint8_t>((value < 0.0f) ? 0.0f : (value > 255.0f) ? 0.0f : value);

            draw_pixel(values_draw, i, k, color(intensity, intensity, intensity));
            //int mult = 1;
            //draw_rectangle(i * mult, j * mult, mult, mult, color(col, col, col));
        }
    }
    return 0;
}