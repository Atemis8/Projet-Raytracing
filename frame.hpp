#include <unordered_map>
#include <forward_list>
#include "structs.hpp"

int frame(std::unordered_map<int, int> *colors, 
            std::forward_list<Wall> *walls,
            std::forward_list<Ray> *rays, 
            std::forward_list<PosVector> *debug_points);

int imgui_test();