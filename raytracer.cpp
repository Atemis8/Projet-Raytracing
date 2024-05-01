#include "raytracer.hpp"

#include <iostream>
#include <cmath>
#include <complex>

#define MINCOLOR 0x000000FF

using namespace std;

PosVector getImage(Wall *w,  PosVector *emitter) {
    const float prod = -2 * ((*emitter - w->v1) * w->n);
    return PosVector(w->n.x *  prod + emitter->x, w->n.y * prod + emitter->y);
}

complex<float> transCoef(Wall &w, PosVector &d) {
    float normd = sqrt(d.getNorm());
    float prod = (d * w.v) / w.size; prod *= prod;
    float cost = sqrt(1 - prod / ((w.mat.sepsr * normd) * (w.mat.sepsr * normd)));
    float s = -2 * w.mat.thck / cost;

    complex<float> ratio = w.mat.imped * (float) ((d * w.n) / (normd * cost));
    complex<float> coefp = (ratio - Z0) / (ratio + Z0); coefp *= coefp;
    complex<float> args = complex<float>(w.mat.gammam.real() * s, s * (w.mat.gammam.imag() - BETA * prod / (normd * normd * w.mat.sepsr)));
    complex<float> coef = exp(args);
    complex<float> out = ((1.0f - coefp) * exp(s / 2 * w.mat.gammam)) / (1.0f - coefp * coef);

    return out;
}

complex<float> reflCoef(Wall &w, PosVector &d) {
    float normd = sqrt(d.getNorm());
    float prod = (d * w.v) / w.size; prod *= prod;
    float cost = sqrt(1 - prod / ((w.mat.sepsr * normd) * (w.mat.sepsr * normd)));
    float s = -2 * w.mat.thck / cost;

    complex<float> ratio = w.mat.imped * (float) ((d * w.n) / (normd * cost));
    complex<float> coefp = (ratio - Z0) / (ratio + Z0);
    complex<float> args = complex<float>(w.mat.gammam.real() * s, s * (w.mat.gammam.imag() - BETA * prod / (normd * normd * w.mat.sepsr)));
    complex<float> coef = exp(args);
    complex<float> out = (coefp - (1.0f - coefp * coefp) * (coefp * coef) / (1.0f - coefp * coefp * coef));
    
    return out;
}

complex<float> reflCoefDet(Wall &w, PosVector &d) {
    clock_t t1 = clock();
    float nomd = sqrt(d.getNorm());
    float nomv = sqrt(w.v.getNorm());
    float cosi = abs(d * w.n) / nomd;
    float sini = abs(d * w.v) / nomd / nomv;
    float sint = sqrt(1 / (w.mat.sepsr * w.mat.sepsr)) * sini;
    float cost = sqrt(1 - sint * sint);
    float s = w.mat.thck / cost;

    // cout << "Detailed Reflection Coefficient : " << endl;

    // cout << "Sin i = " << sini << endl;
    // cout << "Cos i = " << cosi << endl;
    // cout << "Sin t = " << sint << endl;
    // cout << "Cos t = " << cost << endl;
    // cout << "s = " << s << endl;

    complex<float> ratio = w.mat.imped * cosi / (float) (Z0 * cost);
    complex<float> perrc = (ratio - 1.0f) / (ratio + 1.0f);

    // cout << "coefp = " << perrc << endl;
    complex<float> coef1 = exp(- 2 * s * w.mat.gammam);
    complex<float> coef2 = polar(1.0f, (float) (2.0f * BETA * s * sint * sini));
    complex<float> out = perrc - (1.0f - perrc * perrc) * (perrc * coef1 * coef2) / (1.0f - perrc * perrc * coef1 * coef2);
    clock_t t2 = clock();
    cout << "Time : " << t2 - t1 << endl;
    return out;
}

complex<float> transCoefDet(Wall &w, PosVector &d) {
    float nomd = sqrt(d.getNorm());
    float nomv = sqrt(w.v.getNorm());
    float cosi = abs(d * w.n) / nomd;
    float sini = abs(d * w.v) / nomd / nomv;
    float sint = sqrt(1 / (w.mat.sepsr * w.mat.sepsr)) * sini;
    float cost = sqrt(1 - sint * sint);
    float s = w.mat.thck / cost;

    cout << "Detailed Transmission Coefficient : " << endl;

    cout << "Sin i = " << sini << endl;
    cout << "Cos i = " << cosi << endl;
    cout << "Sin t = " << sint << endl;
    cout << "Cos t = " << cost << endl;
    cout << "s = " << s << endl;

    complex<float> ratio = w.mat.imped * cosi / (float) (Z0 * cost);
    complex<float> perrc = (ratio - 1.0f) / (ratio + 1.0f);

    cout << "coefp = " << perrc << endl;
    complex<float> coef1 = exp(- 2 * s * w.mat.gammam);
    complex<float> coef2 = polar(1.0f, (float) (4.0f * PI * FREQ / C * s * sint * sini));
    
    return (1.0f - perrc * perrc) * exp(- s * w.mat.gammam) / (1.0f - perrc * perrc * coef1 * coef2);
}

bool intersection(PosVector *out, PosVector *rx, PosVector *tx, Wall *w) {
    // two points should be on opposite side of the wall
    // lets add a check using a scalar product

    const PosVector d = PosVector(rx->x - tx->x, rx->y - tx->y);
    const float t = - (d.y * (tx->x - w->v1.x) - d.x * (tx->y - w->v1.y)) 
                    / (d.x * w->v.y - d.y * w->v.x);
   
    if(t < 0 || t > 1 || t != t) return 1;
    *(out) = PosVector(w->v1.x + t * w->v.x, w->v1.y + t * w->v.y);
    return 0;
}

bool operator==(Wall& w, Wall& v) {
    return ((v.v1 == w.v1) && (v.v2 == w.v2));
}

void solve(vector<Wall> *walls, Wall *w, PosVector *r, PosVector *t, forward_list<Ray> *wrays, forward_list<PosVector> *dpts, int refl) {
    // First test if a reflection with the given wall can happen for the given reciever and emitter
    if(((*r - w->v1) * w->n) * ((*t - w->v1) * w->n) < 0) return;

    // Finds the vectorial image of the emitter
    PosVector tx = getImage(w, t);
    dpts->push_front(tx);
    PosVector *p1 = (PosVector*) malloc(sizeof(PosVector)); 

    // Checks if the intersection makes physical sense and gives back the intersection point
    if(intersection(p1, r, &tx, w)) return; 

    // Adds the distance of the ray and adds the intersection point and the reciver in the points list
    forward_list<PosVector> pts;
    Ray ray = {.points = pts, .distsq = (*r - tx).getNorm(), .color = static_cast<int>(0xFF0000FF)};
    ray.points.push_front(*r);
    ray.points.push_front(*p1);
    wrays->push_front(ray);
    
    // Recursive part of the function to call for sub emitters calculations
    if(refl > 1)
        for(Wall &v : *walls) {
            if(v == *w) continue; // First checks to that we dont have the same wall twice
            forward_list<Ray> ur; // List with all the future subrays that will be calulated
            solve(walls, &v, r, &tx, &ur, dpts, refl - 1); // Solves to find subrays for each wall
            if(ur.empty()) continue; // If no subrays where found skips to the next wall

            // If subrays where found calculated the points with the current emitter, if the reflection is not physical, remove the ray
            auto before = ur.before_begin();
            for(auto r = ur.begin(); r != ur.end();) {
                PosVector *p3 = (PosVector*) malloc(sizeof(PosVector));
                if(((r->points.front() - w->v1) * w->n) * ((tx - w->v1) * w->n) > 0 
                    ||  intersection(p3, &(r->points.front()), &tx, w)) { 
                    r = ur.erase_after(before); 
                    continue; 
                }
                r->points.push_front(*p3);
                before = r++; free(p3);
            }
            wrays->splice_after(wrays->before_begin(), ur);
        }
}


void traceRays(RaytracerResult *res) {
    int total_rays = 0;
    // For each reciever, evaluates all the necessary rays
    for(int i = 0; i < res->receivers->size(); ++i) {
        for(PosVector em : *res->emitters) { // Iterates over the differents emitters
            forward_list<Ray> rays;

            // Creates the direct path between the reciever and the emitter
            forward_list<PosVector> directPath;
            directPath.push_front((*res->receivers)[i]);
            Ray r = {directPath, ((*res->receivers)[i] - em).getNorm(), static_cast<int>(0xFF0000FF)};
            rays.push_front(r);
            
            // For each wall, solves the vectorial problem
            for(Wall w : *(res->walls)) {
                forward_list<Ray> t;
                solve(res->walls, &w, &(*res->receivers)[i], &em, &t, res->debug_points, res->reflections);
                rays.splice_after(rays.before_begin(), t); // Adds all the rays to the list of rays for a given wall
            }
            
            // Adds the emitter as the last point of the list
            for(Ray &r : rays) {
                r.points.push_front(em);
            }
            total_rays += distance(rays.begin(), rays.end()); // Evaluates the number of rays the code calculated
            (*res->rays)[i].splice_after((*res->rays)[i].before_begin(), rays); // Adds all the rays to the final list
        }
    }
    cout << "Simualtion Complete : " << res->rays->size() << ", Number of rays : " << total_rays << endl;
}