#include "raytracer.hpp"

#include <iostream>
#include <cmath>

forward_list<Ray> traceRays(PosVector emitter, PosVector receiver, forward_list<Wall> *walls) {
    forward_list<Ray> rays;
    forward_list<PosVector> points;

    // Direct Path
    points.push_front(emitter);
    points.push_front(receiver);
    Ray directPath = {.points = points};
    rays.push_front(directPath);

    for(Wall w : *walls) {
        
        // Trouver le point image
        PosVector emiv1 = emitter - w.v1;
        PosVector recv1 = receiver - w.v1;
        const float prod = (-2 / w.size) * (emiv1 * w.n);
        PosVector tx = PosVector(w.n.x *  prod + emitter.x, w.n.y * prod + emitter.y);
        if((w.n * recv1) * (w.n * emiv1) < 0) continue;

        // Trouver le point de réflexion
        const PosVector d = PosVector(receiver.x - tx.x, receiver.y - tx.y);
        const float t = (d.y * (w.v1.x - tx.x) - d.x * (w.v1.y - tx.y)) / (d.x * w.v.y - d.y * w.v.x);
        const PosVector p = PosVector(w.v1.x + t * w.v.x, w.v1.y + t * w.v.y);

        forward_list<PosVector> point;
        point.push_front(emitter);
        point.push_front(p);
        point.push_front(receiver);

        Ray first = {.points = point};
        rays.push_front(first);
    }

    return rays;
}