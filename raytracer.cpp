#include "raytracer.hpp"

#include <iostream>
#include <cmath>

#define MINCOLOR 0x000000FF

using namespace std;

PosVector getImage(Wall *w,  PosVector *emitter) {
    const float prod = (-2 / w->size) * ((*emitter - w->v1) * w->n);
    return PosVector(w->n.x *  prod + emitter->x, w->n.y * prod + emitter->y);
}

/*
Adds the the point of the intersection between rx tx and a wall to the point list
*/
bool intersection(PosVector *out, PosVector *rx, PosVector *tx, Wall *w) {
    const PosVector d = PosVector(rx->x - tx->x, rx->y - tx->y);
    const float t = - (d.y * (tx->x - w->v1.x) - d.x * (tx->y - w->v1.y)) / (d.x * w->v.y - d.y * w->v.x);
   
    if(t < 0 || t > 1) return 1;
    *(out) = PosVector(w->v1.x + t * w->v.x, w->v1.y + t * w->v.y);
    return 0;
}

PosVector* test(forward_list<Wall> *walls, Wall *w, PosVector *r, PosVector *t, forward_list<Ray> *rays, forward_list<Ray> *wrays, forward_list<PosVector> *dpts, int refl) {
    if(((*r - w->v1) * w->n) * ((*t - w->v1) * w->n) < 0) return NULL;

    PosVector tx = getImage(w, t);
    PosVector *p1 = (PosVector*) malloc(sizeof(PosVector));
    if(intersection(p1, r, &tx, w)) return NULL;

    forward_list<PosVector> pts;
    Ray ray = {.points = pts, .distsq = (*r - tx).getNorm(), .color = rand() + MINCOLOR};
    ray.points.push_front(*r);
    ray.points.push_front(*p1);
    rays->push_front(ray);
    
    if(refl > 1)
        for(Wall v : *walls) {
            if((v.v1 == w->v1) && (v.v2 == w->v2)) continue;
            PosVector *p2 = test(walls, &v, r, &tx, rays, NULL, dpts, refl - 1);
            PosVector *p3 = (PosVector*) malloc(sizeof(PosVector));
            if(p2 == NULL || intersection(p3, p2, &tx, w)) continue;
            free(p3); free(p2);
        }
    return p1;
}





// Pour chaque point image si il faut calculer plus de réflexion 
// il faut les donner comme émetteurs ensuite il faut poser limiter les intersections
// Ne pas rajouter l'émetteur directement
// Garder en variable le nombre de réflexion à encore tracer
// Problème sur la distance parcourue
// Pour chaque TX on a 2 cas : murs et pas murs, cas mur uniquement si on a refl > 0

forward_list<Ray> traceRays(PosVector emitter, PosVector receiver, forward_list<Wall> *walls, forward_list<PosVector> *dpts) {
    forward_list<Ray> rays;
    

    for(Wall w : *walls) {
        test(walls, &w, &receiver, &emitter, &rays, NULL, dpts, 2);
    }
    return rays;

    std::cout << std::endl << "Old Version : " << std::endl;

    // Direct Path
    forward_list<PosVector> points;
    points.push_front(emitter);
    points.push_front(receiver);
    Ray directPath = {.points = points, .distsq = (emitter - receiver).getNorm()};
    rays.push_front(directPath);

    for(Wall w : *walls) {
        // Vérifie si le point image et physique et le calcule
        if(((receiver - w.v1) * w.n) * ((emitter - w.v1) * w.n) < 0) continue;
        
        PosVector tx = getImage(&w, &emitter);
        dpts->push_front(tx);

        PosVector * p1 = (PosVector*) malloc(sizeof(PosVector));
        if(intersection(p1, &receiver, &tx, &w)) continue;

        forward_list<PosVector> point;
        Ray r1 = {.points = point, .distsq = 0, .color = rand() + MINCOLOR};

        r1.points.push_front(emitter);
        r1.points.push_front(*p1);
        r1.points.push_front(receiver);
        rays.push_front(r1);

        for(Wall v: *walls) {
            if((w.v1 == v.v1) && (w.v2 == v.v2)) continue;
            if(((receiver - v.v1) * v.n) * ((tx - v.v1) * v.n) < 0) continue;

            PosVector txp = getImage(&v, &tx);
            dpts->push_front(txp);
            
            PosVector * p2 = (PosVector*) malloc(sizeof(PosVector));
            PosVector * p3 = (PosVector*) malloc(sizeof(PosVector));
            if(intersection(p2, &receiver, &txp, &v)) continue;   
            if(intersection(p3, p2, &tx, &w)) continue;

            forward_list<PosVector> points;
            Ray r = {.points = points, .distsq = 0, .color = rand() + MINCOLOR};

            r.points.push_front(receiver);
            r.points.push_front(*p2);
            r.points.push_front(*p3);
            r.points.push_front(emitter);
            rays.push_front(r);
            // std::cout << "Reflection tier 2" << " Vecteur : "; txp.print();
        }
    }

    return rays;
}