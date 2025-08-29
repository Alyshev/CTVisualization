#pragma once
#include <vector>
#include <cmath>
#include "drawing_functions.h"
#include "structs.h"
#include <iostream>

const float max_ray_length = 800.0f;
const float sensitivity = 0.05f;      // чувствительность вращения
const float zoom_speed = 5.0f;        // шаг изменения радиуса
const float pitch_limit = 1.5f;       // предел поворогта

Vector3f normalize(Vector3f v);
Camera make_camera(Vector3f pos, Vector3f look_at, Vector3f up_hint);
bool intersect_ray_box(const Vector3f& ray_origin, const Vector3f& ray_dir, float& tmin, float& tmax, int& X, int& Y, int& Z);
float dot(const Vector3f& a, const Vector3f& b);
Camera make_orbit_camera(Camera cam, Vector3i size_CP, float yaw, float pitch_cam, float radius);
int render_frame(values_by_draw values_draw, values_by_materials* cur_mat, std::vector<uint16_t> cube_pixels, Camera cam, Vector3i size_CP, int step_size, float yaw, float pitch_cam, float radius);
std::vector<std::vector<uint8_t>> render_frame_thread(values_by_draw values_draw, std::vector<values_by_materials> values_mat, int cur_mat_index, std::vector<uint16_t> cube_pixels, Camera cam, Vector3i size_CP, int step_size, float yaw, float pitch_cam, float radius);