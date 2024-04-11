// Dear ImGui: standalone example application for SDL2 + OpenGL
// (SDL is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

#include "imgui/imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include "imguistyle.hpp"
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_opengl.h>

#include <unordered_map>
#include <forward_list>
#include <iostream>
#include "structs.hpp"
#include "main.hpp"

#define RED 0xFF0000FF
#define GREEN 0x00FF00FF
#define BLUE 0x0000FFFF

// Percent of the size as the origin point
static float ORIGINX = 0.1F;
static float ORIGINY = 0.1F;
static float SCALE = 4.0f;

#define POINT_SCALE 4.0f

ImU32 getColor(int color) {
    const int red = (color & 0xFF000000) >> 24;
	const int green = (color & 0x00FF0000) >> 16;
	const int blue = (color & 0x0000FF00) >> 8;
	const int alpha = (color & 0x000000FF);
    return IM_COL32(red, green, blue, alpha);
}


static ImVec2 size;
static ImVec2 cursor;
ImVec2 setAxis(PosVector *v) {
	return ImVec2(ORIGINX * size.x +  v->getX() * SCALE + cursor.x, size.y * (1 - ORIGINY) - v->getY() * SCALE + cursor.y);
}

//AddPolyline(const ImVec2* points, int num_points, ImU32 col, ImDrawFlags flags, float thickness);

void drawRay(Ray *r, ImDrawList* draw_list, int nb) {
    ImVec2* points = (ImVec2*) malloc(sizeof(ImVec2) * nb);
    int i = 0;
    for(PosVector p : r->points) {
        points[i] = setAxis(&p);
        ++i;
    }
    draw_list->AddPolyline(points, nb, getColor(r->color), 0, 1);
    free(points);
}

void drawRaytracer(RaytracerResult *result, RaytracerOptions *options) {
    ImGui::Begin("Raytracer");
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    cursor = ImGui::GetCursorScreenPos();
    size = ImGui::GetWindowSize();

    draw_list->AddCircleFilled(setAxis(result->emitter), POINT_SCALE, getColor(GREEN));
    draw_list->AddCircleFilled(setAxis(result->receiver), POINT_SCALE, getColor(GREEN));

    for(Ray &r : *(result->rays)) {
        int nb = distance(r.points.begin(), r.points.end());
        if(options->selectReflection && (options->nbReflections + 2 != nb)) continue;
        drawRay(&r, draw_list, nb);
    }
    for(Wall w : *(result->walls)) 
        draw_list->AddLine(setAxis(&w.v1), setAxis(&w.v2), options->colors->at(w.mat.id), 2);
    
    if(options->show_debug)
    for(PosVector v : *(result->debug_points))
        draw_list->AddCircleFilled(setAxis(&v), POINT_SCALE, getColor(RED));

    ImGui::Dummy(ImVec2(500, 500));
    ImGui::End();
}

// Main code
int imgui_test(RaytracerResult *result, RaytracerOptions *options) {
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
    // GL 3.2 Core + GLSL 150
    const char* glsl_version = "#version 150";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

    // From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow("Dear ImGui SDL2+OpenGL3 example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
    if (window == nullptr)
    {
        printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        return -1;
    }

    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.Fonts->AddFontFromFileTTF("verdana.ttf", 18.0f, NULL, NULL);
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    // setImGuiStyle();
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Our state
    bool show_demo_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    bool done = false;
    while (!done) {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            
            static int counter = 0;
            static int ref_option = 1;

            ImGui::Begin("Options");  
            ImGui::Text("Create a new Raytracing problem :"); 
            ImGui::InputInt("Reflections", &ref_option);
            if(ref_option < 1) ref_option = 1;
            if(ImGui::Button("Start Simulation")) {
                solveProblem(result, ref_option);
            }      
            
            ImGui::Text("Current Simulation Options : ");
            ImGui::Checkbox("Select Reflection", &(options->selectReflection));
            if(options->selectReflection)
                ImGui::SliderInt("Reflections", &(options->nbReflections), 0.0f, result->reflections);    // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::Checkbox("Show Debug Points", &(options->show_debug));
            ImGui::SliderFloat("Origin X", &ORIGINX, 0.0f, 1.0f);
            ImGui::SliderFloat("Origin Y", &ORIGINY, 0.0f, 1.0f);
            ImGui::SliderFloat("Scale", &SCALE, 1.0f, 10.0f);

            ImGui::Checkbox("Demo Window", &show_demo_window);
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button")) counter++;                 // Buttons return true when clicked (most widgets return true when edited/activated) 
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        drawRaytracer(result, options);

        // Rendering
        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}