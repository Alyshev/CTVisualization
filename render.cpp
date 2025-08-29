#include "render.h"

Vector3f normalize(Vector3f v) {
    float len = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    return { v.x / len, v.y / len, v.z / len };
}

Vector3f cross(const Vector3f& a, const Vector3f& b) {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

Camera make_camera(Vector3f pos, Vector3f look_at, Vector3f up_hint) {
    Vector3f forward = normalize(look_at - pos);
    Vector3f right = normalize(cross(forward, up_hint));
    Vector3f up = cross(right, forward);
    return { pos, forward, up, right };
}

bool intersect_ray_box(const Vector3f& ray_origin, const Vector3f& ray_dir, float& tmin, float& tmax, int& X, int& Y, int& Z) {
    const Vector3f box_min = { 0.0f, 0.0f, 0.0f };
    const Vector3f box_max = { float(X), float(Y), float(Z) };

    tmin = -INFINITY;
    tmax = INFINITY;

    for (int i = 0; i < 3; ++i) {
        float origin = (&ray_origin.x)[i];
        float dir = (&ray_dir.x)[i];
        float minb = (&box_min.x)[i];
        float maxb = (&box_max.x)[i];

        if (fabs(dir) < 1e-6f) {
            if (origin < minb || origin > maxb) return false;
        }
        else {
            float t1 = (minb - origin) / dir;
            float t2 = (maxb - origin) / dir;
            if (t1 > t2) std::swap(t1, t2);
            tmin = std::max(tmin, t1);
            tmax = std::min(tmax, t2);
            if (tmin > tmax) return false;
        }
    }
    return true;
}

float dot(const Vector3f& a, const Vector3f& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Camera make_orbit_camera(Camera cam, Vector3i size_CP, float yaw, float pitch_cam, float radius) {
    const Vector3f center = { size_CP.x / 2.0f, size_CP.y / 2.0f, size_CP.z / 2.0f };

    pitch_cam = std::max(std::min(pitch_cam, 1.5f), -1.5f);  // в радианах (~±86°)

    float x = center.x + radius * cos(pitch_cam) * sin(yaw);
    float y = center.y + radius * sin(pitch_cam);
    float z = center.z + radius * cos(pitch_cam) * cos(yaw);
    cam.pos = { x, y, z };

    cam.forward = normalize(center - cam.pos);

    Vector3f world_up = { 0.0f, 1.0f, 0.0f };

    if (fabs(dot(cam.forward, world_up)) > 0.999f) {
        world_up = { 0.0f, 0.0f, 1.0f };
    }

    cam.right = normalize(cross(cam.forward, world_up));
    cam.up = normalize(cross(cam.right, cam.forward));

    return cam;
}

int render_frame(values_by_draw values_draw, values_by_materials* cur_mat, std::vector<uint16_t> cube_pixels, Camera cam, Vector3i size_CP, int step_size, float yaw, float pitch_cam, float radius)
{

    cam = make_orbit_camera(cam, size_CP, yaw, pitch_cam, radius);

    for (int y = 0; y < values_draw.screen_height; ++y) {
        for (int x = 0; x < values_draw.screen_width; ++x) {
            float u = (x - values_draw.screen_width / 2.0f) / values_draw.screen_width;
            float v = (y - values_draw.screen_height / 2.0f) / values_draw.screen_height;

            Vector3f ray_origin = cam.pos + cam.right * u * static_cast<float>(size_CP.x) + cam.up * v * static_cast<float>(size_CP.y);
            Vector3f ray_dir = cam.forward;

            float t0, t1;
            if (!intersect_ray_box(ray_origin, ray_dir, t0, t1, size_CP.x, size_CP.y, size_CP.z)) {
                draw_pixel(values_draw, x, y, color(0, 0, 0));
                continue;
            }
            else {
                //draw_pixel(x, y, color(255, 255, 255));
                //continue;
            }

            float start_t = std::max(t0, 0.0f);
            float end_t = std::min(t1, start_t + max_ray_length);
            int steps = (int)((end_t - start_t) / step_size);

            Vector3f pos = ray_origin + ray_dir * start_t;
            //int steps = (int)((t1 - std::max(t0, 0.0f)) / step_size);

            uint8_t max_intensity = 0;

            for (int i = 0; i < steps; ++i) {
                int xi = static_cast<int>(round(pos.x));
                int yi = static_cast<int>(round(pos.y));
                int zi = static_cast<int>(round(pos.z));
                if (xi >= 0 && xi < size_CP.x && yi >= 0 && yi < size_CP.y && zi >= 0 && zi < size_CP.z) {
                    uint16_t value = cube_pixels[static_cast<size_t>(zi) * size_CP.y * size_CP.x + static_cast<size_t>(yi) * size_CP.x + static_cast<size_t>(xi)];
                    const float scale = 255.0f / (cur_mat->max_vis_win - (cur_mat->min_vis_win));
                    value = static_cast<int>((value - (cur_mat->min_vis_win)) * scale + 0.5f);
                    const uint8_t intensity = static_cast<uint8_t>((value < 0.0f) ? 0.0f : (value > 255.0f) ? 0.0f : value);
                    if (intensity > max_intensity)
                    {
                        max_intensity = intensity;
                    }
                }
                pos = pos + ray_dir * static_cast<float>(step_size);
            }

            draw_pixel(values_draw, x, y, color(max_intensity, max_intensity, max_intensity));
        }
    }
    return 0;
}

std::vector<std::vector<uint8_t>> render_frame_thread(values_by_draw values_draw, std::vector<values_by_materials> values_mat, int cur_mat_index, std::vector<uint16_t> cube_pixels, Camera cam, Vector3i size_CP, int step_size, float yaw, float pitch_cam, float radius)
{
    std::vector<std::vector<uint8_t>> image(values_draw.screen_height, std::vector<uint8_t>(values_draw.screen_width));

    cam = make_orbit_camera(cam, size_CP, yaw, pitch_cam, radius);

    for (int y = 0; y < values_draw.screen_height; ++y) {
        for (int x = 0; x < values_draw.screen_width; ++x) {
            float u = (x - values_draw.screen_width / 2.0f) / values_draw.screen_width;
            float v = (y - values_draw.screen_height / 2.0f) / values_draw.screen_height;

            Vector3f ray_origin = cam.pos + cam.right * u * static_cast<float>(size_CP.x) + cam.up * v * static_cast<float>(size_CP.y);
            Vector3f ray_dir = cam.forward;

            float t0, t1;
            if (!intersect_ray_box(ray_origin, ray_dir, t0, t1, size_CP.x, size_CP.y, size_CP.z)) {
                draw_pixel(values_draw, x, y, color(0, 0, 0));
                continue;
            }

            float start_t = std::max(t0, 0.0f);
            float end_t = std::min(t1, start_t + max_ray_length);
            int steps = (int)((end_t - start_t) / step_size);

            Vector3f pos = ray_origin + ray_dir * start_t;
            uint8_t max_intensity = 0;

            for (int i = 0; i < steps; ++i) {
                int xi = static_cast<int>(round(pos.x));
                int yi = static_cast<int>(round(pos.y));
                int zi = static_cast<int>(round(pos.z));
                if (xi >= 0 && xi < size_CP.x && yi >= 0 && yi < size_CP.y && zi >= 0 && zi < size_CP.z) {
                    uint16_t value = cube_pixels[static_cast<size_t>(zi) * size_CP.y * size_CP.x + static_cast<size_t>(yi) * size_CP.x + static_cast<size_t>(xi)];
                    const float scale = 255.0f / (values_mat[cur_mat_index].max_vis_win - (values_mat[cur_mat_index].min_vis_win));
                    value = static_cast<int>((value - (values_mat[cur_mat_index].min_vis_win)) * scale + 0.5f);
                    const uint8_t intensity = static_cast<uint8_t>((value < 0.0f) ? 0.0f : (value > 255.0f) ? 0.0f : value);
                    if (intensity > max_intensity)
                    {
                        max_intensity = intensity;
                    }
                }
                pos = pos + ray_dir * static_cast<float>(step_size);
            }

            image[y][x] = max_intensity;
        }
    }
    return image;
}