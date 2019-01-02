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
            num_objects = std::stoi(line);

            positions.reserve(num_objects);
            colours.reserve(num_objects);

            for ( int i = 0; i < num_objects && myfile.good(); i++ ) {
                getline(myfile, line);
                //positions.push_back({ 0.0f, 0.0f, std::stof(line), 1.0f});
                positions.push_back({ 0.0f, 0.0f, 0.0f, 1.0f});
                colours.push_back({ 1.0f, 1.0f, 1.0f, 0 });
            }
            for ( int i = 0; i < num_objects && myfile.good(); i++ ) {
                getline(myfile, line);
                positions[i].pos.x = std::stof(line);
            }
            for ( int i = 0; i < num_objects && myfile.good(); i++ ) {
                getline(myfile, line);
                positions[i].pos.y = std::stof(line);
            }

            myfile.close();
            /*
            // Normalise

            auto minmax_x = std::minmax_element(positions.begin(), positions.end(),
                                                [] (ObjectPositions const& lhs, ObjectPositions const& rhs) {return lhs.pos.x < rhs.pos.x;});
            auto minmax_y = std::minmax_element(positions.begin(), positions.end(),
                                                [] (ObjectPositions const& lhs, ObjectPositions const& rhs) {return lhs.pos.y < rhs.pos.y;});
            auto minmax_z = std::minmax_element(positions.begin(), positions.end(),
                                                [] (ObjectPositions const& lhs, ObjectPositions const& rhs) {return lhs.pos.z < rhs.pos.z;});

            auto max_x = minmax_x.second->pos.x;
            auto max_y = minmax_y.second->pos.y;
            auto max_z = minmax_z.second->pos.z;
            double divisor = std::pow( 10.0, int(std::log10(std::max(max_x, std::max(max_y, max_z))))+1 );

            std::for_each(positions.begin(), positions.end(), [=](ObjectPositions &n){
                n.pos.x /= divisor;
                n.pos.x *= 10000;
                n.pos.y /= divisor;
                n.pos.y *= 10000;
                n.pos.z /= divisor;
            });
            */
            cout << "Loaded";

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

