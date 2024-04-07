#include <complex>
#include <forward_list>
#include "vector.hpp"

#define FREQ 60E9

#ifndef utilitystructs

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

#define utilitystructs 
#endif