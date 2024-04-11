#include <iostream>
#include <unordered_map>
#include <cstdlib>
#include <time.h> 
      
#include "main.hpp"
#include "vector.hpp"
#include "structs.hpp"
#include "frame.hpp"
#include "raytracer.hpp"
#include "imguitest.hpp"

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
	PosVector v = v2 - v1;
    Wall w = {.mat = mat, .v1 = v1, .v2 = v2, .v = v, .n = v.normal(), .size = v.getNorm()};
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
	m->insert(pair<int, int>(brick.id, 0xFF0000FF));
	m->insert(pair<int, int>(concr.id, 0x808080FF));
	m->insert(pair<int, int>(wallm.id, 0xFFA500FF));
	m->insert(pair<int, int>(glass.id, 0xFFFFFFFF));
	m->insert(pair<int, int>(metal.id, 0xd3d3d3FF));
}

void initWalls() {
	addWall(concr, PosVector(0.0F, 0.0F), PosVector(0.0F, 80.0F));
	addWall(wallm, PosVector(0.0F, 20.0F), PosVector(100.0F, 20.0F));
	addWall(glass, PosVector(0.0F, 80.0F), PosVector(100.0F, 80.0F));
}

static PosVector emitter = PosVector(32.0F, 10.0F);
static PosVector receiver = PosVector(47.0F, 65.0F);
static forward_list<PosVector> debug_points;
static forward_list<Ray> rays;
void solveProblem(RaytracerResult *res, int reflections) {
	rays.clear();
	debug_points.clear();
	*(res) = {&emitter, &receiver, &walls, &debug_points, &rays, reflections};
	traceRays(res);
}

int main(int argc, char *argv[]) {
	srand(time(NULL));

	forward_list<PosVector> list1;
	list1.push_front(PosVector(10, 10));
	list1.push_front(PosVector(20, 10));

	unordered_map<int, int> colorMap;
	initColorMap(&colorMap);
	initWalls();

	RaytracerResult result;
	RaytracerOptions options = {&colorMap, 0, 1, 1};
	
	solveProblem(&result, 2);
	imgui_test(&result, &options);
	return 0;
}



/*
forward_list<int> list1;
forward_list<int> list2;

list1.push_front(2);
list1.push_front(1);

list2.push_front(4);
list2.push_front(3);

list2.splice_after(list2.before_begin(), list1);

cout << "List 1 : " << endl;
for(int i : list1) cout << i << endl;
cout << "List 2 : " << endl;
for(int i : list2) cout << i << endl;
*/