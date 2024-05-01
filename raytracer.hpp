#include <forward_list>

#include "structs.hpp"

void traceRays(RaytracerResult *res);
complex<float> reflCoef(Wall &w, PosVector &d);
complex<float> reflCoefDet(Wall &w, PosVector &d);
complex<float> transCoef(Wall &w, PosVector &d);
complex<float> transCoefDet(Wall &w, PosVector &d);
