#include "structs.h"

Vector2i::Vector2i(int x_val, int y_val) : x(x_val), y(y_val) {}
Vector2i Vector2i::operator+(const Vector2i& other) const
{
	return Vector2i(x + other.x, y + other.y);
}
void Vector2i::operator+=(const Vector2i& other)
{
	x += other.x; y += other.y;
}
Vector2i Vector2i::operator-(const Vector2i& other) const
{
	return Vector2i(x - other.x, y - other.y);
}
void Vector2i::operator-=(const Vector2i& other)
{
	x -= other.x; y -= other.y;
}

Vector2f::Vector2f(float x_val, float y_val) : x(x_val), y(y_val) {}
Vector2f Vector2f::operator+(const Vector2f& other) const
{
	return Vector2f(x + other.x, y + other.y);
}
void Vector2f::operator+=(const Vector2f& other)
{
	x += other.x; y += other.y;
}
Vector2f Vector2f::operator-(const Vector2f& other) const
{
	return Vector2f(x - other.x, y - other.y);
}
void Vector2f::operator-=(const Vector2f& other)
{
	x -= other.x; y -= other.y;
}
Vector2f Vector2f::operator*(const float& fl) const
{
	return Vector2f(x * fl, y * fl);
}

Vector3f::Vector3f(float x_val, float y_val, float z_val) : x(x_val), y(y_val), z(z_val) {}
Vector3f Vector3f::operator+(const Vector3f& other) const
{
	return Vector3f(x + other.x, y + other.y, z + other.z);
}
void Vector3f::operator+=(const Vector3f& other)
{
	x += other.x; y += other.y; z += other.z;
}
Vector3f Vector3f::operator-(const Vector3f& other) const
{
	return Vector3f(x - other.x, y - other.y, z - other.z);
}
void Vector3f::operator-=(const Vector3f& other)
{
	x -= other.x; y -= other.y; z -= other.z;
}
Vector3f Vector3f::operator*(const float& fl) const
{
	return Vector3f(x * fl, y * fl, z * fl);
}


Vector3i::Vector3i(int x_val, int y_val, int z_val) : x(x_val), y(y_val), z(z_val) {}




