#include <iostream>
#include <unordered_map>
      
#include "main.hpp"
#include "vector.hpp"
#include "structs.hpp"
#include "frame.hpp"

using namespace std;

static int id = 0;
static unordered_map<int, Material> matmap;
Material material(float relp, float cond, float thck) {
	const Material m = {.id = id, .relp = relp, .cond=cond, .thck=thck, .perm=std::complex<float>(relp, -cond / FREQ)};
    matmap[id] = m; ++id;
    return m;
};

static forward_list<Wall> walls;
Wall* addWall(Material mat, PosVector v1, PosVector v2) {
    Wall w = {.mat = mat, .v1 = v1, .v2 = v2};
    walls.push_front(w);
    return &w;
}

const Material test = material(4.8, 0.018, 0.15);
const Material brick = material(3.95, 0.073, 0.1); // Pas d'infos sur les briques dans notre simulation
const Material concr = material(6.4954, 1.43, 0.3);
const Material wallm = material(2.7, 0.05346, 0.1);
const Material glass = material(6.3919, 0.00107, 0.05);
const Material metal = material(1, 1E7, 0.05);

void initColorMap(unordered_map<int, int> *m) { 
	m->insert(pair<int, int>(brick.id, 0xFF0000));
	m->insert(pair<int, int>(concr.id, 0x808080));
	m->insert(pair<int, int>(wallm.id, 0xFFA500));
	m->insert(pair<int, int>(glass.id, 0xFFFFFF));
	m->insert(pair<int, int>(metal.id, 0xd3d3d3));
}

void initWalls() {
	addWall(concr, PosVector(0, 0), PosVector(0, 80));
	addWall(concr, PosVector(0, 20), PosVector(100, 20));
	addWall(concr, PosVector(0, 80), PosVector(100, 80));
}

int main(int argc, char *argv[]) {
	unordered_map<int, int> colorMap;
	initColorMap(&colorMap);
	initWalls();

	cout << "Permitivité : " << metal.perm << endl;

	frame(&colorMap, &walls);
	return 0;
}

