#include "structs.hpp"

int imgui_test(RaytracerResult *result, RaytracerOptions *options);

void setDefaultOptions(RaytracerOptions *options, unordered_map<int, int> *colorMap, 
            unordered_map<int, Material> *matmap, int mat_nb, 
            vector<PosVector> *emitters, vector<PosVector> *receivers);