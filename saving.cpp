#include <fstream>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include "structs.hpp"
#include "main.hpp"

using namespace std;

static string filepath = "saves/walls_test.dat";

void save_problem(vector<Wall> *walls, vector<PosVector> *emitters) {
    if(mkdir("saves", 0777) == -1) cout << "No need to create directory" << endl;
    ofstream outputFile(filepath);
    if(outputFile.is_open()) {
        for(Wall w : *walls) 
            outputFile << w.mat.id << ";" << w.v1 << ";" << w.v2 << endl;
        outputFile.close();
    } else cout << "File is not opened" << endl;
}

void load_problem(vector<PosVector> *emitters, unordered_map<int, Material> *matmap) {
    ifstream inputFile(filepath);
    if(inputFile.is_open()) {
        string s;
        int matid;
        PosVector v1, v2;
        char space;
        while(inputFile) {
            getline(inputFile, s);
            if(s.size() == 0) continue;
            istringstream line(s);
            line >> matid;
            line >> space;
            line >> v1;
            line >> space;
            line >> v2;
            addWall(matmap->at(matid), v1, v2);
        } 
    } else cout << "File Problem" << endl;
}