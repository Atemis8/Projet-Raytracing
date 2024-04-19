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
#include "saving.hpp"

using namespace std;

static int mat_id = 0;
static unordered_map<int, Material> matmap; // Maybe changing it to froward list
static complex<float> mu0 = complex<float>(MU0, 0);
Material material(string name, float epsr, float cond, float thck) {
	complex<float> perm = complex<float>(epsr * EPSILON0, -cond / (FREQ * 2 * PI));
	const Material m = {.id = mat_id, .epsr = epsr, .cond=cond, .thck=thck, .perm=perm, .imped=sqrt(mu0 / perm), .name = name};
    matmap[mat_id] = m; 
	++mat_id;
    return m;
}

static vector<Wall> walls;
void addWall(Material mat, PosVector v1, PosVector v2) {
	PosVector v = v2 - v1;
	float size = sqrt(v.getNorm());
    Wall w = {.mat = mat, .v1 = v1, .v2 = v2, .v = v, .n = (v.normal()) / size, .size = (size)};
	walls.push_back(w);
}

const Material test = material("Test", 4.8, 0.018, 0.15);
const Material brick = material("Brick", 3.95, 0.073, 0.1); // Pas d'infos sur les briques dans notre simulation
const Material concr = material("Concrete", 6.4954, 1.43, 0.3);
const Material wallm = material("Partition", 2.7, 0.05346, 0.1);
const Material glass = material("Glass", 6.3919, 0.00107, 0.05);
const Material metal = material("Metal", 1, 1E7, 0.05);

void initColorMap(unordered_map<int, int> *m) { 
	m->insert(pair<int, int>(test.id, 0xFF0F0FFF));
	m->insert(pair<int, int>(brick.id, 0xFF0000FF));
	m->insert(pair<int, int>(concr.id, 0x808080FF));
	m->insert(pair<int, int>(wallm.id, 0xFFA500FF));
	m->insert(pair<int, int>(glass.id, 0xFFFFFFFF));
	m->insert(pair<int, int>(metal.id, 0xd3d3d3FF));
}

static forward_list<PosVector> debug_points;
static vector<forward_list<Ray>> rays;
void solveProblem(RaytracerResult *res, vector<PosVector> *emitters, vector<PosVector> *receivers, int reflections) {
	rays.clear();
	debug_points.clear();
	time_t time1 = time(NULL);
	*(res) = {
		.emitters = emitters, 
		.receivers = receivers,
		.walls = &walls,
		.debug_points = &debug_points,
		.rays = &rays,
		.reflections = reflections
	};
	rays = vector<forward_list<Ray>>(receivers->size());
	traceRays(res);
	time_t time2 = time(NULL);
	cout << ((float) (time2 - time1)) << "ms" << endl;
}


int main(int argc, char *argv[]) {
	srand(time(NULL));

	forward_list<PosVector> list1;
	list1.push_front(PosVector(10, 10));
	list1.push_front(PosVector(20, 10));
	list1.push_front(PosVector(30, 10));
	
	unordered_map<int, int> colorMap;
	initColorMap(&colorMap);

	vector<PosVector> emitters;
	vector<PosVector> receivers;
	load_problem(&emitters, &matmap);

	RaytracerResult result;
	RaytracerOptions options;
	setDefaultOptions(&options, &colorMap, &matmap, mat_id, &emitters, &receivers);
	solveProblem(&result, &options.emitters, &options.receivers, 3);

	imgui_test(&result, &options);
	return 0;
}

/*
for(Wall w : walls) w.n.print();
for(int i = 0; i < mat_id; ++i)
	cout << matmap[i].name << " : " << matmap[i].imped << endl;
*/