#include "structs.hpp"
void solveProblem(RaytracerResult *res, forward_list<PosVector> *emitters, int reflections);
void addWall(Material mat, PosVector v1, PosVector v2);