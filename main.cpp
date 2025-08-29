// Dear ImGui: standalone example application for SDL3 + SDL_Renderer
// (SDL is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

// Important to understand: SDL_Renderer is an _optional_ component of SDL3.
// For a multi-platform app consider using e.g. SDL+DirectX on Windows and SDL+OpenGL on Linux/OSX.

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_sdl3.h"
#include "ImGui/imgui_impl_sdlrenderer3.h"
#include <stdio.h>
#include <SDL3/SDL.h>
#include <random>
#include <fstream>
#include <string>
#include <map>
#include <thread>
#include <chrono>

#include "drawing_functions.h"
#include "render.h"
#include "data_processing.h"
#include "draw_projections.h"

#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL3/SDL_opengles2.h>
#else
#include <SDL3/SDL_opengl.h>
#endif

#ifdef __EMSCRIPTEN__
#include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif

void loop_thread(double& fps, values_by_draw& values_draw, std::vector<std::vector<uint8_t>>& image, bool & flag_render, bool& flag_thread, std::vector<values_by_materials>& values_mat, int& cur_mat_index, std::vector<uint16_t>& cube_pixels, Camera& cam, Vector3i& size_CP, int& step_size, float& yaw, float& pitch_cam, float& radius)
{
    using clock = std::chrono::high_resolution_clock;
    auto lastTime = clock::now();
    int frames = 0;

    while (flag_thread)
    {
        if (flag_render)
        {
            image = render_frame_thread(values_draw, values_mat, cur_mat_index, cube_pixels, cam, size_CP, step_size, yaw, pitch_cam, radius);

            frames++;

            auto now = clock::now();
            std::chrono::duration<double> elapsed = now - lastTime;

            if (elapsed.count() >= 1.0) {
                fps = frames / elapsed.count();

                frames = 0;
                lastTime = now;
            }
        }
    }
}

int main(int, char**)
{

    values_by_draw values_draw;

    int count_materials = get_count_materials_in_file();
    std::vector<values_by_materials> values_mat;
    values_mat.reserve(count_materials);
    for (int i = 0; i < count_materials; i++) {
        if (i >= 0 && i < DEFAULT_VALUES_BY_MATERIALS.size()) {
            values_mat.push_back(DEFAULT_VALUES_BY_MATERIALS[i]);
        }
        else {
            values_mat.push_back(values_by_materials(i));
        }
    }
    int cur_mat_index = 0; //current_material_index = default num 0(1)
    values_by_materials* cur_mat = &values_mat[cur_mat_index]; //current_material
    static int current_projection_index = static_cast<int>(values_mat[cur_mat_index].projection);

    std::vector<std::string> material_names;
    std::vector<const char*> material_pointers;
    set_materials(material_names, material_pointers, count_materials);


    Vector3i size_CP(0, 0, 0);
    std::vector<uint16_t> cube_pixels;
    read_proc(size_CP, cube_pixels, cur_mat->index);

    Camera cam = make_camera({ float(size_CP.x / 2), float(size_CP.y / 2), float(-100) }, { float(size_CP.x / 2), float(size_CP.y / 2), float(size_CP.z / 2) }, { float(0), float(1), float(0) });

    int step_size = 40;             // accuracy of calculations for rendering
    float yaw = 0.0f;               // вращение по горизонтали
    float pitch_cam = 0.0f;         // вверх/вниз
    float radius = 300.0f;          // расстояние от центра

    const char* PROJECTIONS_NAMES[] = { "Top", "Right", "Front", "Render (WASD)" };

    // Setup SDL
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
    {
        printf("Error: SDL_Init(): %s\n", SDL_GetError());
        return -1;
    }

    // Create window with SDL_Renderer graphics context
    Uint32 window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;
    int screenWidth = 720, screenHeight = 720;
    SDL_Window* window = SDL_CreateWindow("CTVisualization", screenWidth, screenHeight, window_flags);
    if (window == nullptr)
    {
        printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        return -1;
    }
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    SDL_SetRenderVSync(renderer, 1);
    if (renderer == nullptr)
    {
        SDL_Log("Error: SDL_CreateRenderer(): %s\n", SDL_GetError());
        return -1;
    }
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_ShowWindow(window);

    SDL_GetWindowSize(window, &screenWidth, &screenHeight);
    SDL_Texture* target_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
        screenWidth, screenHeight);
    if (target_texture == nullptr)
    {
        SDL_Log("Error: SDL_CreateTexture(): %s\n", SDL_GetError());
        return -1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    bool done = false;

    UpdateBuffer(values_draw, screenWidth, screenHeight);

    float color_background[3] = { 0.0f, 0.0f, 0.0f };

    SDL_Event event;

    // Render loop
    bool flag_render = true;
    bool flag_thread = true;
    double fps_thread = 0;
    std::vector<std::vector<uint8_t>> image(values_draw.screen_height, std::vector<uint8_t>(values_draw.screen_width, 0));
    std::thread thread_render(loop_thread, std::ref(fps_thread), std::ref(values_draw), std::ref(image), std::ref(flag_render), std::ref(flag_thread), std::ref(values_mat), std::ref(cur_mat_index), std::ref(cube_pixels), std::ref(cam), std::ref(size_CP), std::ref(step_size), std::ref(yaw), std::ref(pitch_cam), std::ref(radius));
    thread_render.detach();

    clearScreen(values_draw, color(0 * 255, 0 * 255, 0 * 255, 255));

#ifdef __EMSCRIPTEN__
    // For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
    // You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
    io.IniFilename = nullptr;
    EMSCRIPTEN_MAINLOOP_BEGIN
#else
    while (!done)
#endif
    {
        //event handling
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL3_ProcessEvent(&event);

            if (event.type == SDL_EVENT_QUIT)
                done = true;
            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window))
                done = true;
            if (event.type == SDL_EVENT_WINDOW_RESIZED)
            {
                int newWidth, newHeight;
                SDL_GetWindowSize(window, &newWidth, &newHeight);
                UpdateBuffer(values_draw, newWidth, newHeight);

                SDL_DestroyTexture(target_texture);
                target_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, newWidth, newHeight);

                image = std::vector<std::vector<uint8_t>>(values_draw.screen_height, std::vector<uint8_t>(values_draw.screen_width, 0));

                clearScreen(values_draw, color(0 * 255, 0 * 255, 0 * 255, 255));
            }
            if (event.type == SDL_EVENT_KEY_DOWN)
            {
                switch (event.key.scancode)
                {
                case SDL_SCANCODE_ESCAPE:
                    done = true;

                case SDL_SCANCODE_W:
                    pitch_cam += sensitivity;
                    if (pitch_cam > 1.5f) pitch_cam = 1.5f;
                    break;

                case SDL_SCANCODE_S:
                    pitch_cam -= sensitivity;
                    if (pitch_cam < -1.5f) pitch_cam = -1.5f;
                    break;

                case SDL_SCANCODE_A: // влево
                    yaw -= sensitivity;
                    break;

                case SDL_SCANCODE_D: // вправо
                    yaw += sensitivity;
                    break;

                case SDL_SCANCODE_R: // приближение
                    radius -= zoom_speed;
                    if (radius < 10.0f) radius = 10.0f; // минимальное расстояние
                    break;

                case SDL_SCANCODE_E: // отдаление
                    radius += zoom_speed;
                    break;

                default:
                    break;
                }
            }

        }
        if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
        {
            SDL_Delay(10);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();


        //UI
        ImGui::Begin("Settings");

        if (cur_mat->projection == Projection::Render) {
            ImGui::Text("(Application: %.0f FPS)\t(Render: %.2f FPS)", io.Framerate, fps_thread);
        }
        else {
            ImGui::Text("(Application: %.0f FPS)", io.Framerate);
        }

        ImGui::SliderInt("MinVisualization window", &cur_mat->min_vis_win, 1, cur_mat->max_vis_win);
        if (cur_mat->min_vis_win >= cur_mat->max_vis_win) { cur_mat->max_vis_win = cur_mat->min_vis_win; }

        ImGui::SliderInt("MaxVisualization window", &cur_mat->max_vis_win, cur_mat->min_vis_win, 2000);

        if (cur_mat->projection == Projection::Render) {
            ImGui::SliderInt("Accuracy", &step_size, 5, 50);
        }

        if (cur_mat->projection != Projection::Render) {
            if (cur_mat->projection == Projection::Top)
            {
                ImGui::SliderInt("Shift", &cur_mat->shift, 0, size_CP.z - 1);
            }
            else if (cur_mat->projection == Projection::Right)
            {
                ImGui::SliderInt("Shift", &cur_mat->shift, 0, size_CP.x - 1);
            }
            else if (cur_mat->projection == Projection::Front)
            {
                ImGui::SliderInt("Shift", &cur_mat->shift, 0, size_CP.y - 1);
            }
            else if (cur_mat->projection == Projection::Render)
            {
                ImGui::SliderInt("Shift", &cur_mat->shift, 0, size_CP.z - 1);
            }
        }

        if (ImGui::Combo("Select Option", &cur_mat_index, material_pointers.data(), material_pointers.size()))
        {
            if (values_mat[cur_mat_index].projection == Projection::Render) {
                flag_render = false;
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                flag_render = true;
            }
            else { flag_render = false; }

            cur_mat = &values_mat[cur_mat_index];
            read_proc(size_CP, cube_pixels, cur_mat->index);
            current_projection_index = static_cast<int>(cur_mat->projection);

            clearScreen(values_draw, color(0 * 255, 0 * 255, 0 * 255, 255));
        }

        if (ImGui::Combo("Projection", &current_projection_index,
            PROJECTIONS_NAMES, IM_ARRAYSIZE(PROJECTIONS_NAMES)))
        {
            cur_mat->projection = static_cast<Projection>(current_projection_index);
            if (cur_mat->projection == Projection::Top) { cur_mat->shift = size_CP.z / 2; }
            else if (cur_mat->projection == Projection::Right) { cur_mat->shift = size_CP.x / 2; }
            else if (cur_mat->projection == Projection::Front) { cur_mat->shift = size_CP.y / 2; }
            else if (cur_mat->projection == Projection::Render) { cur_mat->shift = size_CP.z / 2; }

            if (cur_mat->projection == Projection::Render) { flag_render = true; }
            else { flag_render = false; }

            clearScreen(values_draw, color(0 * 255, 0 * 255, 0 * 255, 255));
        }

        if (cur_mat->projection == Projection::Render) {
            ImGui::Text("Controls on WASD");
        }

        ImGui::End();


        // Rendering
        ImGui::Render();
        SDL_SetRenderDrawColorFloat(renderer, clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        SDL_RenderClear(renderer);

        //clearScreen(color(color_background[0] * 255, color_background[1] * 255, color_background[2] * 255, 100));
        //clearScreen(values_draw, color(0 * 255, 0 * 255, 0 * 255, 255));


        if (cur_mat->projection == Projection::Top) 
        {
            draw_projection_top(values_draw, size_CP, cube_pixels, cur_mat);
        }
        else if (cur_mat->projection == Projection::Right) 
        {
            draw_projection_right(values_draw, size_CP, cube_pixels, cur_mat);
        }
        else if (cur_mat->projection == Projection::Front) 
        {
            draw_projection_front(values_draw, size_CP, cube_pixels, cur_mat);
        }
        else if (cur_mat->projection == Projection::Render) 
        {
            //render_frame(cur_mat, cube_pixels, cam, size_CP, step_size, yaw, pitch_cam, radius);
            for (int y = 0; y < image.size(); y++) {
                for (int x = 0; x < image[0].size(); x++) {
                    draw_pixel(values_draw, x, y, color(image[y][x], image[y][x], image[y][x]));
                }
            }
        }

        //draw_rectangle(0, 0, screen_width / 2, screen_height / 2, color(15, 57, 60, 200));
        //draw_rectangle(screen_width / 3, screen_height / 3, screen_width / 4, screen_height / 4, color(100, 0, 20, 180));
        //draw_rectangle(screen_width / 4, screen_height / 4, screen_width / 1.5, screen_height / 1.5, color(33, 120, 66, 100));
        //draw_rectangle(screen_width / 5, screen_height / 5, screen_width / 3, screen_height / 3, color(6, 87, 90, 150));

        //draw_circle(Vector2f(screen_width / 5, screen_width / 5), 200, color(150, 157, 60, 200));

        SDL_UpdateTexture(target_texture, nullptr, values_draw.data, values_draw.pitch);
        SDL_RenderTexture(renderer, target_texture, nullptr, nullptr);
        



        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
    }
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_END;
#endif

    delete[] values_draw.data;


    // Cleanup
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
