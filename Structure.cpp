//
// Created by lf on 09/12/18.
//

#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>

#include <glm/common.hpp>
#include <iomanip>


#include "Structure.h"


using namespace std;


std::vector<Structure::ObjectPositions> Structure::getObjectPositions() { return positions; }

std::vector<Structure::ObjectColours> Structure::getObjectColours() { return colours; }

Structure::Structure( std::string filename ) {

    loadFile(filename);

    srand(time(NULL));


    MapObjectToColor();
    //PrintPoints();
    CalcSystemEnergy();
}


void Structure::PrintPoints() {
    for (int i = 0; i < positions.size(); i++) {
        XYZW_GL pt = positions[i].pos;
        fprintf(stdout,
                "%d:[%f,%f,%f] ",
                i, pt.x, pt.y, pt.z);
    }
    fprintf(stdout, "\n");
}

void Structure::MapObjectToColor() {

    // not yet
}


void Structure::CalcSystemEnergy() {

    // not yet
}


void Structure::loadFile( string filename ) {

    if (!filename.empty()) {

        string line;
        ifstream myfile(filename);
        if (myfile.is_open()) {

            getline(myfile, line);
            while (myfile.good()) {
                if (line.size() > 0) {
                    // read lines

                }

                getline(myfile, line);
            }
            myfile.close();
        } else {
            cout << "Unable to open file";
        }

    } else {

        unsigned gridsize = 100;
        num_objects = gridsize * gridsize;

        positions.reserve(num_objects);
        colours.reserve(num_objects);

        for ( unsigned i = 0; i < gridsize; i++ ) {
            for ( unsigned j = 0; j < gridsize; j++ ) {
                positions.push_back({ -0.5f + (float(i)/gridsize), -0.5f + (float(j)/gridsize), 0, 1});
                colours.push_back({ 1.0f, 1.0f, 1.0f, 0 });
            }
        }
    }
}

