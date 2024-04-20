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
    const PosVector d = PosVector(rx->x - tx->x, rx->y - tx->y);
    const float t = - (d.y * (tx->x - w->v1.x) - d.x * (tx->y - w->v1.y)) 
                    / (d.x * w->v.y - d.y * w->v.x);
   
    if(t < 0 || t > 1) return 1;
    *(out) = PosVector(w->v1.x + t * w->v.x, w->v1.y + t * w->v.y);
    return 0;
}

void test(vector<Wall> *walls, Wall *w, PosVector *r, PosVector *t, forward_list<Ray> *wrays, forward_list<PosVector> *dpts, int refl) {
    if(((*r - w->v1) * w->n) * ((*t - w->v1) * w->n) < 0) return;

    PosVector tx = getImage(w, t);
    dpts->push_front(tx);
    PosVector *p1 = (PosVector*) malloc(sizeof(PosVector));
    if(intersection(p1, r, &tx, w)) return;

    forward_list<PosVector> pts;
    Ray ray = {.points = pts, .distsq = (*r - tx).getNorm(), .color = static_cast<int>(0xFF0000FF)};
    ray.points.push_front(*r);
    ray.points.push_front(*p1);

    wrays->push_front(ray);
    
    if(refl > 1)
        for(Wall v : *walls) {
            if((v.v1 == w->v1) && (v.v2 == w->v2)) continue;
            forward_list<Ray> ur;
            test(walls, &v, r, &tx, &ur, dpts, refl - 1);
            if(ur.empty()) continue;
            auto before = ur.before_begin();
            for(auto r = ur.begin(); r != ur.end();) {
                PosVector *p3 = (PosVector*) malloc(sizeof(PosVector));
                if(intersection(p3, &(r->points.front()), &tx, w)) { r = ur.erase_after(before); continue; }
                r->points.push_front(*p3);
                before = r++; free(p3);
            }
            wrays->splice_after(wrays->before_begin(), ur);
        }
}

// Pour chaque point image si il faut calculer plus de réflexion 
// il faut les donner comme émetteurs ensuite il faut poser limiter les intersections
// Ne pas rajouter l'émetteur directement
// Garder en variable le nombre de réflexion à encore tracer
// Problème sur la distance parcourue
// Pour chaque TX on a 2 cas : murs et pas murs, cas mur uniquement si on a refl > 0

void traceRays(RaytracerResult *res) {
    for(int i = 0; i < res->receivers->size(); ++i) {
        for(PosVector em : *res->emitters) {
            forward_list<Ray> rays;
            forward_list<PosVector> directPath;
            directPath.push_front((*res->receivers)[i]);
            Ray r = {directPath, ((*res->receivers)[i] - em).getNorm(), static_cast<int>(0xFF0000FF)};
            rays.push_front(r);
            
            for(Wall w : *(res->walls)) {
                forward_list<Ray> t;
                test(res->walls, &w, &(*res->receivers)[i], &em, &t, res->debug_points, res->reflections);
                rays.splice_after(rays.before_begin(), t);
            }
            
            for(Ray &r : rays) {
                r.points.push_front(em);
            }
            (*res->rays)[i].splice_after((*res->rays)[i].before_begin(), rays);
        }
    }
    cout << "Simualtion Complete : " << res->rays->size() << endl;
}