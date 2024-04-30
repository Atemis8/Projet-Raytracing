#include <complex>
#include <forward_list>
#include <unordered_map>
#include <string>
#include <vector>
#include "vector.hpp"
#include "imgui/imgui.h"

#define FREQ 868E6
#define PI 3.1415926535897932f
#define MU0 (PI * 4E-7)
#define EPSILON0 8.85418782E-12
#define Z0 376.730313668

#ifndef utilitystructs

using namespace std;

struct Material {
    float epsr;
    float cond;
    float thck;
    int id;
    string name;
    complex<float> perm;
    complex<float> imped;
};

struct Wall {
    Material mat;
    PosVector v1;
    PosVector v2;
    float size;
    PosVector v;
    PosVector n;
};

struct Ray {
    std::forward_list<PosVector> points;
    float distsq;
    int color;
};

struct RaytracerResult {
    vector<PosVector> *emitters;
    vector<PosVector> *receivers;
    vector<Wall> *walls;
    forward_list<PosVector> *debug_points;
    vector<forward_list<Ray>> *rays;
    int reflections;
};

struct RaytracerOptions {
    unordered_map<int, int> *colors;
    unordered_map<int, Material> *matmap;
    
    int nbReflections;
    bool selectReflection;

    int selected_ray;
    bool select_ray;

    bool show_debug;
    bool show_demo;
    ImVec4 background_color;
    bool show_walleditor;
    bool show_raytracer;
    int mat_number;
    bool show_evalzone;
    PosVector evalO;
    PosVector evalZ;
    int eval_size;
    vector<PosVector> emitters;
    vector<PosVector> receivers;
};

#define utilitystructs 
#endif