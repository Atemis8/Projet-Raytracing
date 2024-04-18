#include <fstream>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include "structs.hpp"
#include "main.hpp"

using namespace std;

void save_problem(vector<Wall> *walls, forward_list<PosVector> *emitters) {
    if(mkdir("saves", 0777) == -1) cout << "No need to create directory" << endl;
    ofstream outputFile("saves/walls.dat");
    if(outputFile.is_open()) {
        for(Wall w : *walls) 
            outputFile << w.mat.id << ";" << w.v1 << ";" << w.v2 << endl;
        outputFile.close();
    } else cout << "File is not opened" << endl;
}

void load_problem(forward_list<PosVector> *emitters, unordered_map<int, Material> *matmap) {
    ifstream inputFile("saves/walls.dat");
    if(inputFile.is_open()) {
        string s;
        int matid;
        PosVector v1, v2;
        char space;
        while(inputFile) {
            getline(inputFile, s);
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