
#include "structs.hpp"

void save_problem(vector<Wall> *walls, forward_list<PosVector> *emitters);
void load_problem(forward_list<PosVector> *emitters, unordered_map<int, Material> *matmap);