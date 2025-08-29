#pragma once
#include <iostream>

enum class Projection {
	Top,
	Right,
	Front,
	Render
};

struct values_by_materials {
	int index, shift, min_vis_win, max_vis_win;
	Projection projection;
	values_by_materials(int index, int shift = 0, int min_vis_win = 1, int max_vis_win = 100, Projection projection = Projection::Front)
		: index(index), shift(shift), min_vis_win(min_vis_win), max_vis_win(max_vis_win), projection(projection) {}
};

struct values_by_draw {
	uint8_t* data;
	int screen_width, screen_height;
	unsigned int pitch;
	values_by_draw(int screen_width = 720, int screen_height = 720, unsigned int pitch = 720 * 4)
		: screen_width(screen_width), screen_height(screen_height), pitch(pitch) {
		data = new uint8_t[static_cast<size_t>(screen_width) * static_cast<size_t>(screen_height) * 4]();
	}
};

struct Vector2i
{
	int x;
	int y;
	Vector2i(int x_val, int y_val);

	Vector2i operator+(const Vector2i& other) const;
	void operator+=(const Vector2i& other);
	Vector2i operator-(const Vector2i& other) const;
	void operator-=(const Vector2i& other);
};

struct Vector2f
{
	float x;
	float y;
	Vector2f(float x_val, float y_val);

	Vector2f operator+(const Vector2f& other) const;
	void operator+=(const Vector2f& other);
	Vector2f operator-(const Vector2f& other) const;
	void operator-=(const Vector2f& other);

	Vector2f operator*(const float& fl) const;
};

struct Vector3f
{
	float x;
	float y;
	float z;
	Vector3f(float x_val, float y_val, float z_val);

	Vector3f operator+(const Vector3f& other) const;
	void operator+=(const Vector3f& other);
	Vector3f operator-(const Vector3f& other) const;
	void operator-=(const Vector3f& other);
	Vector3f operator*(const float& fl) const;
};

struct Vector3i
{
	int x;
	int y;
	int z;
	Vector3i(int x_val, int y_val, int z_val);
};

struct color{
	uint8_t r, g, b, a;
	color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) : r(r), g(g), b(b), a(a) {}
};

struct Camera {
	Vector3f pos;
	Vector3f forward;
	Vector3f up;
	Vector3f right;
};