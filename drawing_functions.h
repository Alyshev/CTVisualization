#pragma once

#include <cstdint>
#include "structs.h"

void UpdateBuffer(values_by_draw& values_draw, const int& newWidth, const int& newHeight);
void draw_pixel(values_by_draw& values_draw, int const& pX, int const& pY, const color& c);
void clearScreen(values_by_draw& values_draw, color const& c);
void draw_line(values_by_draw& values_draw, int x0, int y0, const int& x1, const int& y1, const color& c);
void draw_rectangle(values_by_draw& values_draw, int const& x0, int const& y0, int const& xSize, int const& ySize, const color& c);
void draw_circle(values_by_draw& values_draw, int const& xc, int const& yc, int const& radius, const color& c);
void draw_circle(values_by_draw& values_draw, const Vector2f& pos, int const& radius, const color& c);



