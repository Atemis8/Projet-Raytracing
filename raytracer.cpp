#include "raytracer.hpp"

#include <iostream>
#include <cmath>

#define MINCOLOR 0x000000FF

using namespace std;

PosVector getImage(Wall *w,  PosVector *emitter) {
    const float prod = -2 * ((*emitter - w->v1) * w->n);
    return PosVector(w->n.x *  prod + emitter->x, w->n.y * prod + emitter->y);
}

complex<float> transCoef(Wall &w, PosVector &d, float gamma) {
    complex<float> rc = reflCoef(w, d);
    complex<float> rcs = rc * rc;

    return rc;
}

complex<float> reflCoef(Wall &w, PosVector &d) {
    float prod = d * w.v;
    complex<float> ratio = w.mat.imped * (float) ((d * w.n) / (Z0 * sqrt(d.getNorm() - prod * prod / (w.mat.epsr * w.size * w.size))));
    return ((ratio - 1.0f) / (ratio + 1.0f));
}

bool intersection(PosVector *out, PosVector *rx, PosVector *tx, Wall *w) {
    // two points should be on opposite side of the wall
    // lets add a check using a scalar product

    const PosVector d = PosVector(rx->x - tx->x, rx->y - tx->y);
    const float t = - (d.y * (tx->x - w->v1.x) - d.x * (tx->y - w->v1.y)) 
                    / (d.x * w->v.y - d.y * w->v.x);
   
    if(t < 0 || t > 1 || t != t) return 1;
    *(out) = PosVector(w->v1.x + t * w->v.x, w->v1.y + t * w->v.y);

    cout << "RX : " << *rx << ", TX : " << *tx << ", OUT : " << *out << ", t : " << t << endl;
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