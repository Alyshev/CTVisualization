#include "drawing_functions.h"

void UpdateBuffer(values_by_draw& values_draw, const int & newWidth, const int& newHeight) {
    values_draw.screen_width = newWidth;
    values_draw.screen_height = newHeight;
    values_draw.pitch = values_draw.screen_width * 4;

    delete[] values_draw.data;
    values_draw.data = new uint8_t[static_cast<size_t>(values_draw.screen_width) * static_cast<size_t>(values_draw.screen_height) * 4]();

    //std::cout << "Updated size window: " << values_draw.screen_width << "x" << values_draw.screen_height << std::endl;
}

void draw_pixel(values_by_draw& values_draw, int const& pX, int const& pY, color const& c)
{
    if (pX >= 0 && pX < values_draw.screen_width && pY >= 0 && pY < values_draw.screen_height)
    {
        unsigned int const pixel_first_byte_index= values_draw.pitch * pY + pX * 4 ;

        float src_alpha = c.a / 255.0f;
        float dest_alpha = values_draw.data[pixel_first_byte_index + 0] / 255.0f;
        float out_alpha = src_alpha + dest_alpha * (1 - src_alpha);

        if (out_alpha > 0)
        {
            values_draw.data[pixel_first_byte_index + 1] = static_cast<uint8_t>((c.b * src_alpha + values_draw.data[pixel_first_byte_index + 1] * dest_alpha * (1 - src_alpha)) / out_alpha);
            values_draw.data[pixel_first_byte_index + 2] = static_cast<uint8_t>((c.g * src_alpha + values_draw.data[pixel_first_byte_index + 2] * dest_alpha * (1 - src_alpha)) / out_alpha);
            values_draw.data[pixel_first_byte_index + 3] = static_cast<uint8_t>((c.r * src_alpha + values_draw.data[pixel_first_byte_index + 3] * dest_alpha * (1 - src_alpha)) / out_alpha);
        }
        values_draw.data[pixel_first_byte_index + 0] = static_cast<uint8_t>(out_alpha * 255);
    }
}

void clearScreen(values_by_draw& values_draw, color const& c)
{
    for (int i = 0; i < values_draw.screen_width * values_draw.screen_height * 4; i += 4)
    {
        values_draw.data[i + 0] = c.a;
        values_draw.data[i + 1] = c.b;
        values_draw.data[i + 2] = c.g;
        values_draw.data[i + 3] = c.r;
    }
}

void draw_line(values_by_draw& values_draw, int x0, int y0, const int& x1, const int& y1, const color& c)
{
    const int dx = abs(x1 - x0);
    const int sx = x0 < x1 ? 1 : -1;
    const int dy = -abs(y1 - y0);
    const int sy = y0 < y1 ? 1 : -1;


    if (dx == 0)
    {
        while (true)
        {
            draw_pixel(values_draw, x0, y0, c);
            if (x0 == x1 && y0 == y1) break;
            y0 += sy;
        }
    }
    else if (dy == 0)
    {
        while (true)
        {
            draw_pixel(values_draw, x0, y0, c);
            if (x0 == x1 && y0 == y1) break;
            x0 += sx;
        }
    }
    else
    {
        int error = dx + dy;
        int e2;
        while (true)
        {
            draw_pixel(values_draw, x0, y0, c);
            if (x0 == x1 && y0 == y1) break;
            e2 = 2 * error;
            if (e2 >= dy)
            {
                error += dy;
                x0 += sx;
            }
            if (e2 <= dx)
            {
                error += dx;
                y0 += sy;
            }
        }
    }
}

void draw_rectangle(values_by_draw& values_draw, int const& x0, int const& y0, int const& xSize, int const& ySize, const color& c)
{
    for (int x = x0; x < x0 + xSize; x++)
    {
        for (int y = y0; y < y0 + ySize; y++)
        {
            draw_pixel(values_draw, x, y, c);
        }
    }
}

void draw_circle(values_by_draw& values_draw, int const& xc, int const& yc, int const& radius, const color& c)
{
    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;

    while (y >= x) {
        draw_pixel(values_draw, xc + x, yc + y, c);
        draw_pixel(values_draw, xc - x, yc + y, c);
        draw_pixel(values_draw, xc + x, yc - y, c);
        draw_pixel(values_draw, xc - x, yc - y, c);
        draw_pixel(values_draw, xc + y, yc + x, c);
        draw_pixel(values_draw, xc - y, yc + x, c);
        draw_pixel(values_draw, xc + y, yc - x, c);
        draw_pixel(values_draw, xc - y, yc - x, c);

        x++;
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        }
        else {
            d = d + 4 * x + 6;
        }
    }
}

void draw_circle(values_by_draw& values_draw, const Vector2f& pos, int const& radius, const color& c)
{
    draw_circle(values_draw, static_cast<int>(pos.x), static_cast<int>(pos.y), radius, c);
}
