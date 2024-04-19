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
#include "saving.hpp"
#include <memory>
#include <stdexcept>
#include <iterator>

#define RED 0xFF0000FF
#define GREEN 0x00FF00FF
#define BLUE 0x0000FFFF

// Percent of the size as the origin point
static float ORIGINX = 0.1F;
static float ORIGINY = 0.1F;
static float SCALE = 4.0f;

#define POINT_SCALE 4.0f



template<typename ... Args>
std::string string_format( const std::string& format, Args ... args ) {
    int size_s = std::snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
    if( size_s <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
    auto size = static_cast<size_t>( size_s );
    std::unique_ptr<char[]> buf( new char[ size ] );
    std::snprintf( buf.get(), size, format.c_str(), args ... );
    return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
}

template<typename ... Args>
void char_format(char*& dest, const std::string& format, Args ... args) {
    int size_s = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1;
    dest = (char*) malloc(size_s);
    std::snprintf(dest, size_s, format.c_str(), args ...);
}

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

ImVec2 setAxis(int x, int y) {
	return ImVec2(ORIGINX * size.x +  x * SCALE + cursor.x, size.y * (1 - ORIGINY) - y * SCALE + cursor.y);
}

void setDefaultOptions(RaytracerOptions *options, unordered_map<int, int> *colorMap, 
            unordered_map<int, Material> *matmap, int mat_nb, 
            vector<PosVector> *emitters, vector<PosVector> *receivers) {
   *(options) = {
        .colors = colorMap,
        .matmap = matmap, 
        .nbReflections = 0,
        .selectReflection = 1,
        .show_debug = 0,
        .show_demo = 0,
        .background_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f), 
        .show_walleditor = 0,
        .show_raytracer = 1,
        .mat_number = mat_nb,
        .show_evalzone = 0,
        .evalO = PosVector(0, 0),
        .evalZ = PosVector(10, 10),
        .eval_size = 10,
        .emitters = *(emitters),
        .receivers = *(receivers)
    };
    options->emitters.push_back(PosVector(32.0f, 10.0f));
    options->emitters.push_back(PosVector(50.0f, 50.0f));
    options->receivers.push_back(PosVector(47.0F, 65.0F));
    options->receivers.push_back(PosVector(80.0F, 30.0F));
}

//AddPolyline(const ImVec2* points, int num_points, ImU32 col, ImDrawFlags flags, float thickness);

void drawRay(Ray *r, ImDrawList* draw_list, int nb) {
    auto p = r->points.begin();
    for(int i = 0; i < nb - 1; ++i)
        draw_list->AddLine(setAxis(&(*p)), setAxis(&(*++p)), getColor(r->color));
}

static int receiver_index = 0;
void drawRaytracer(RaytracerResult *result, RaytracerOptions *o) {
    ImGui::Begin("Raytracer");
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    cursor = ImGui::GetCursorScreenPos();
    size = ImGui::GetWindowSize();
    
    for(PosVector v : *result->emitters)
        draw_list->AddCircleFilled(setAxis(&v), POINT_SCALE, getColor(GREEN));

    draw_list->AddCircleFilled(setAxis(&((*result->receivers)[receiver_index])), POINT_SCALE, getColor(GREEN));
    for(Ray &r : (*result->rays)[receiver_index]) {
        int nb = distance(r.points.begin(), r.points.end());
        if(o->selectReflection && (o->nbReflections + 2 != nb)) continue;
        drawRay(&r, draw_list, nb);
    }
    
    for(Wall w : *(result->walls))
        draw_list->AddLine(setAxis(&w.v1), setAxis(&w.v2), o->colors->at(w.mat.id), 2);
    if(o->show_debug)
    for(PosVector v : *(result->debug_points))
        draw_list->AddCircleFilled(setAxis(&v), POINT_SCALE, getColor(RED));
    if(o->show_evalzone) {
        PosVector evalZone = (o->evalZ * (float)(o->eval_size));
        draw_list->AddRect(setAxis(&o->evalO), setAxis(&evalZone), getColor(BLUE));
        for(int x = o->evalO.x; x < evalZone.x; x += o->eval_size)
            draw_list->AddLine(setAxis(x, o->evalO.y), setAxis(x, evalZone.y), getColor(BLUE), 0.1f);
        for(int y = o->evalO.y; y < evalZone.y; y += o->eval_size)
            draw_list->AddLine(setAxis(o->evalO.x, y), setAxis(evalZone.x, y), getColor(BLUE), 0.1f);
    }
    ImGui::Dummy(ImVec2(500, 500));
    ImGui::End();
}

void drawOptions(RaytracerResult *result, RaytracerOptions *o) {
    static int counter = 0;
    static int ref_option = 1;
    ImGui::Begin("Options");  

    // Problem Creation
    ImGui::Text("Create a new Raytracing problem :"); 
    ImGui::InputInt("Reflections", &ref_option);
    if(ref_option < 1) ref_option = 1;
    if(ImGui::Button("Start Simulation"))
        solveProblem(result, &o->emitters, &o->receivers, ref_option);

    // Evaluation Zone (ie : Zone where the calculations take place)
    ImGui::Text("Evaluation Zone");
    ImGui::Checkbox("Show Zone", &(o->show_evalzone));
    static int vO[2]; vO[0] = o->evalO.x; vO[1] = o->evalO.y;
    if(ImGui::InputInt2("Origin", vO)) {
        o->evalO.x = (float) vO[0];
        o->evalO.y = (float) vO[1];
    }
    static int vZ[2]; vZ[0] = o->evalZ.x; vZ[1] = o->evalZ.y;
    if(ImGui::InputInt2("Zone Number", vZ)) {
        o->evalZ.x = (float) vZ[0];
        o->evalZ.y = (float) vZ[1];
    }
    ImGui::InputInt("Size", &o->eval_size);
    
    // Already existing simulation options
    ImGui::Text("Current Simulation");
    ImGui::SliderInt("Select Receiver", &receiver_index, 0.0f, o->receivers.size() - 1);
    ImGui::Checkbox("Select Reflection", &(o->selectReflection));
    if(o->selectReflection)
        ImGui::SliderInt("Reflections", &(o->nbReflections), 0.0f, result->reflections);    // Edit 1 float using a slider from 0.0f to 1.0f
    ImGui::Checkbox("Show Debug Points", &(o->show_debug));
    ImGui::SliderFloat("Origin X", &ORIGINX, 0.0f, 1.0f);
    ImGui::SliderFloat("Origin Y", &ORIGINY, 0.0f, 1.0f);
    ImGui::SliderFloat("Scale", &SCALE, 1.0f, 10.0f);

    ImGui::ColorEdit3("Background color", (float*)&(o->background_color));
    ImGui::Checkbox("Demo Window", &(o->show_demo));
    const string s = string_format("%s Wall Editor", (o->show_walleditor ? "Hide" : "Show"));
    if(ImGui::Button(s.c_str())) o->show_walleditor = !o->show_walleditor;
    ImGui::SameLine();
    const string s1 = string_format("%s Raytracer", (o->show_raytracer ? "Hide" : "Show"));
    if(ImGui::Button(s1.c_str())) o->show_raytracer = !o->show_raytracer;

    ImGui::End();
}

static int wall_item = 0;
static int mat_item = 0;
void drawWallEditor(RaytracerResult *res, RaytracerOptions *o) {
    auto fw = res->walls->begin();
    auto nb = distance(fw, res->walls->end());
    char **items = (char **) malloc(nb * sizeof(char*));
    int i = 0; 
    for(Wall &w : *res->walls)
        char_format(items[i++], "%s, (%0.1f, %0.1f) -> (%0.1f, %0.1f)", 
                w.mat.name.c_str(), w.v1.x, w.v1.y, w.v2.x, w.v2.y);
    ImGui::Begin("Wall Editor");  
    ImGui::ListBox("##wall_select", &wall_item, items, nb);
    for(int j = 0; j < i; ++j) free(items[j]); free(items);

    ImGui::Text("Manange Walls");

    // Material list
    char **materials = (char **) malloc(o->mat_number * sizeof(char*));
    for(int j = 0; j < o->mat_number; ++j)
        char_format(materials[j], "%s", o->matmap->at(j).name.c_str());  
    ImGui::Combo("##mats", &mat_item, materials, o->mat_number);
    static float v1[2]; 
    ImGui::InputFloat2("Vector 1", v1);
    static float v2[2]; 
    ImGui::InputFloat2("Vector 2", v2);

    if(ImGui::Button("Remove")) res->walls->erase(res->walls->begin() + wall_item);
    ImGui::SameLine();

    if(ImGui::Button("Add Wall"))
        addWall(o->matmap->at(mat_item), PosVector(v1[0], v1[1]), PosVector(v2[0], v2[1]));
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
        if (options->show_demo)
            ImGui::ShowDemoWindow(&(options->show_demo));

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        drawOptions(result, options);

        if(options->show_raytracer)
            drawRaytracer(result, options);
        if(options->show_walleditor)
            drawWallEditor(result, options);

        // Rendering
        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(options->background_color.x * options->background_color.w, 
                    options->background_color.y * options->background_color.w, 
                    options->background_color.z * options->background_color.w, 
                    options->background_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    // Save files
    save_problem(result->walls, &options->emitters);

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}