#include <iostream>
#include <unordered_map>
#include <cstdlib>
#include <time.h> 
#include <complex>
#include <cmath>

#include "main.hpp"
#include "vector.hpp"
#include "structs.hpp"
#include "frame.hpp"
#include "raytracer.hpp"
#include "imguitest.hpp"
#include "saving.hpp"

using namespace std;

static int mat_id = 0;
static unordered_map<int, Material> matmap;
static unordered_map<int, int> colorMap;
static complex<float> mu0 = complex<float>(MU0, 0);
Material material(string name, float epsr, float cond, float thck, int color) {
	float omega = 2 * PI * FREQ;
	float eps = epsr * EPSILON0;
	float coef1 = omega * sqrt(MU0 * eps / 2);
	float coef2 = sqrt(1 + (cond / omega / eps) * (cond / omega / eps));

	complex<float> perm = complex<float>(eps, -cond / omega);
	const Material m = {
		.id = mat_id, 
		.sepsr = sqrt(epsr),
		.cond=cond, 
		.thck=thck, 
		.perm=perm, 
		.imped=sqrt(mu0 / perm), 
		.name = name,
		.gammam = complex<float>(coef1 * sqrt(coef2 - 1), coef1 * sqrt(coef2 + 1))
	};
    matmap[mat_id] = m; 
	colorMap[mat_id] = color;
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

void init_materials() {
	material("Test", 4.8, 0.018, 0.15, 0xFF0F0FFF);
	material("Brick", 3.95, 0.073, 0.1, 0xFF0000FF);
	material("Concrete", 6.4954, 1.43, 0.3, 0x808080FF);
	material("Partition", 2.7, 0.05346, 0.1, 0xFFA500FF);
	material("Glass", 6.3919, 0.00107, 0.05, 0xFFFFFFFF);
	material("Metal", 1, 1E7, 0.05, 0xd3d3d3FF);
	material("Other", 1, 1, 1, 0x00FFFFFF);
}

static forward_list<PosVector> debug_points;
static vector<forward_list<Ray>> rays;
void solveProblem(RaytracerResult *res, vector<PosVector> *emitters, vector<PosVector> *receivers, int reflections) {
	rays.clear();
	debug_points.clear();
	clock_t begin = clock();
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
	clock_t end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	cout << "Time taken : " << time_spent * 1000 << "ms, " << end - begin << " clocks" << endl;
}


int main(int argc, char *argv[]) {
	srand(time(NULL)); // Init the random
	init_materials(); // Init the different materials

	vector<PosVector> emitters;
	vector<PosVector> receivers;
	load_problem(&emitters, &matmap); // Loads the problem from the save files

	PosVector d = PosVector(79, -85);
	cout << transCoef(walls[2], d) << endl;

	return 0;

	RaytracerResult result;
	RaytracerOptions options;
	cout << "Starting" << endl;
	setDefaultOptions(&options, &colorMap, &matmap, mat_id, &emitters, &receivers); // Sets the default options for the calculations
	solveProblem(&result, &options.emitters, &options.receivers, 2); // Solves the probelm and puts it in the result object

	imgui_test(&result, &options); // Graphical interface for the problem
	return 0;
}