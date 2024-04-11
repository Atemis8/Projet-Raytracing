#include <complex>
#include <forward_list>
#include <unordered_map>
#include "vector.hpp"

#define FREQ 60E9

#ifndef utilitystructs

using namespace std;

typedef struct _Material {
    float relp;
    float cond;
    float thck;
    int id;
    std::complex<float> perm;
} Material;

typedef struct _Wall {
    Material mat;
    PosVector v1;
    PosVector v2;
    const float size;
    const PosVector v;
    PosVector n;
} Wall;

typedef struct _Ray {
    std::forward_list<PosVector> points;
    float distsq;
    int color;
} Ray;

typedef struct _RaytracerResult {
    PosVector *emitter;
    PosVector *receiver;
    forward_list<Wall> *walls;
    forward_list<PosVector> *debug_points;
    forward_list<Ray> *rays;
    int reflections;
} RaytracerResult;

typedef struct _RaytracerOptions {
    unordered_map<int, int> *colors;
    int nbReflections;
    bool selectReflection;
    bool show_debug;
} RaytracerOptions;

#define utilitystructs 
#endif