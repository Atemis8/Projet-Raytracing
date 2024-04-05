#include <forward_list>

#include "structs.hpp"

using namespace std;

forward_list<Ray> traceRays(PosVector emitter, PosVector receiver, forward_list<Wall> *walls);