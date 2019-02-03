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
#include <functional>

#include <glm/common.hpp>
#include <iomanip>


#include "Structure.h"


using namespace std;


std::vector<Structure::Position> &Structure::getPositions() {
    return positions;
}

std::vector<Structure::Colour> &Structure::getColours() {
    return colours;
}

std::vector<Structure::Acceleration> &Structure::getAccelerations() {
    return accelerations;
}

vector<Structure::Mass> &Structure::getMasses() {
    return masses;
}

vector<Structure::Velocity> &Structure::getVelocities() {
    return velocities;
}

vector<Structure::Softening> &Structure::getSoftenings() {
    return softenings;
}

vector<Structure::Potential> &Structure::getPotentials() {
    return potentials;
}



Structure::Structure( std::string filename ) {

    loadFile(filename);

    srand(time(NULL));


    MapObjectToColor();
    //PrintPoints();
    CalcSystemEnergy();

    ready = true;
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

float max_r = 0.0f;

float Structure::meanInterParticleSeparation() {

    // https://en.wikipedia.org/wiki/Mean_inter-particle_distance

    max_r = 0.0f;

    std::for_each(positions.begin(), positions.end(),
        []( Structure::Position &pos ){
            Structure::XYZW_GL p = pos.pos;
            float r = std::sqrt(p.x*p.x + p.y*p.y + p.z*p.z);
            max_r = std::max(max_r, r);
        });

    double V = 4.0 / 3.0 * M_PI * max_r * max_r * max_r;
    double n = positions.size() / V;
    double w = std::pow( (3.0/4.0/M_PI/n), 1.0/3.0 );
    return (float)w;

}


void Structure::loadFile( string filename ) {

    if (!filename.empty()) {

        string line;
        ifstream myfile(filename);
        if (myfile.is_open()) {

            getline(myfile, line);
            num_objects = (unsigned)std::stoi(line);

            masses.reserve(num_objects);
            for ( int i = 0; i < num_objects && myfile.good(); i++ ) {
                getline(myfile, line);
                masses.push_back({std::stof(line)});
            }

            positions.reserve(num_objects);
            colours.reserve(num_objects);
            accelerations.reserve(num_objects);

            for ( int i = 0; i < num_objects && myfile.good(); i++ ) {
                getline(myfile, line);
                positions.push_back({ std::stof(line), 0.0f, 0.0f, 1.0f});
                colours.push_back({ 1.0f, 1.0f, 1.0f, 0 });
                accelerations.push_back({ 0.0f, 0.0f, 0.0f });
            }
            for ( int i = 0; i < num_objects && myfile.good(); i++ ) {
                getline(myfile, line);
                positions[i].pos.y = std::stof(line);
            }
            for ( int i = 0; i < num_objects && myfile.good(); i++ ) {
                getline(myfile, line);
                positions[i].pos.z = std::stof(line);
            }

            velocities.reserve(num_objects);
            double vfactor = 1.0e-9;

            for ( int i = 0; i < num_objects && myfile.good(); i++ ) {
                getline(myfile, line);
                velocities.push_back({ std::stof(line)*vfactor, 0.0f, 0.0f});
            }
            for ( int i = 0; i < num_objects && myfile.good(); i++ ) {
                getline(myfile, line);
                velocities[i].v.y = std::stof(line)*vfactor;
            }
            for ( int i = 0; i < num_objects && myfile.good(); i++ ) {
                getline(myfile, line);
                velocities[i].v.z = std::stof(line)*vfactor;
            }

            softenings.reserve(num_objects);
            for ( int i = 0; i < num_objects && myfile.good(); i++ ) {
                getline(myfile, line);
                softenings.push_back({std::stof(line)});
            }

            potentials.reserve(num_objects);
            for ( int i = 0; i < num_objects && myfile.good(); i++ ) {
                getline(myfile, line);
                potentials.push_back({std::stof(line)});
            }



            myfile.close();

            auto minmax_x = std::minmax_element(positions.begin(), positions.end(),
                                                [] (Structure::Position const& lhs, Structure::Position const& rhs) {return lhs.pos.x < rhs.pos.x;});
            auto minmax_y = std::minmax_element(positions.begin(), positions.end(),
                                                [] (Structure::Position const& lhs, Structure::Position const& rhs) {return lhs.pos.y < rhs.pos.y;});
            auto minmax_z = std::minmax_element(positions.begin(), positions.end(),
                                                [] (Structure::Position const& lhs, Structure::Position const& rhs) {return lhs.pos.z < rhs.pos.z;});

            maxPosition.pos.x = minmax_x.second->pos.x;
            maxPosition.pos.y = minmax_y.second->pos.y;
            maxPosition.pos.z = minmax_z.second->pos.z;

            minPosition.pos.x = minmax_x.first->pos.x;
            minPosition.pos.y = minmax_y.first->pos.y;
            minPosition.pos.z = minmax_z.first->pos.z;

            auto max_x = std::max(std::abs(maxPosition.pos.x), std::abs(minPosition.pos.x));
            auto max_y = std::max(std::abs(maxPosition.pos.y), std::abs(minPosition.pos.y));
            auto max_z = std::max(std::abs(maxPosition.pos.z), std::abs(minPosition.pos.z));

            h = std::ceil(std::max(std::max(max_x, max_y), max_z));



            cout << "Data Load Complete\n";

        } else {
            cout << "Unable to open file" << filename << "\n";
        }

    } else {

        unsigned gridsize = 100;
        num_objects = gridsize * gridsize;

        positions.reserve(num_objects);
        colours.reserve(num_objects);
        accelerations.reserve(num_objects);


        for ( unsigned i = 0; i < gridsize; i++ ) {
            for ( unsigned j = 0; j < gridsize; j++ ) {
                positions.push_back({ -0.5f + (float(i)/gridsize), -0.5f + (float(j)/gridsize), 0, 1});
                colours.push_back({ 1.0f, 1.0f, 1.0f, 0 });
            }
        }
    }
}

const Structure::Position &Structure::getMinPosition() const {
    return minPosition;
}

const Structure::Position &Structure::getMaxPosition() const {
    return maxPosition;
}

void Structure::saveFile( string filename_base ) {

    string filename;
    filename.append(filename_base);
    filename.append("-");

    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y%m%d%H%M%S", &tm);
    filename.append(buffer);

    filename.append(".ascii");

    ofstream myfile(filename);
    if (myfile.is_open()) {

        myfile << num_objects << " 0 " << num_objects << "\n";

        myfile.precision(8);
        myfile << std::fixed;

        for ( int i = 0; i < num_objects && myfile.good(); i++ ) {
            myfile << masses[i].m << "\n";
        }
        for ( int i = 0; i < num_objects && myfile.good(); i++ ) {
            myfile << positions[i].pos.x << "\n";
        }
        for ( int i = 0; i < num_objects && myfile.good(); i++ ) {
            myfile << positions[i].pos.y << "\n";
        }
        for ( int i = 0; i < num_objects && myfile.good(); i++ ) {
            myfile << positions[i].pos.z << "\n";
        }
        for ( int i = 0; i < num_objects && myfile.good(); i++ ) {
            myfile << velocities[i].v.x << "\n";
        }
        for ( int i = 0; i < num_objects && myfile.good(); i++ ) {
            myfile << velocities[i].v.y << "\n";
        }
        for ( int i = 0; i < num_objects && myfile.good(); i++ ) {
            myfile << velocities[i].v.z << "\n";
        }
        for ( int i = 0; i < num_objects && myfile.good(); i++ ) {
            myfile << softenings[i].s << "\n";
        }
        for ( int i = 0; i < num_objects && myfile.good(); i++ ) {
            myfile << potentials[i].pot << "\n";
        }
        for ( int i = 0; i < num_objects && myfile.good(); i++ ) {
            myfile << accelerations[i].a.x << "\n";
        }
        for ( int i = 0; i < num_objects && myfile.good(); i++ ) {
            myfile << accelerations[i].a.y << "\n";
        }
        for ( int i = 0; i < num_objects && myfile.good(); i++ ) {
            myfile << accelerations[i].a.z << "\n";
        }
        myfile.close();

        cout << "File saved: " << filename << "\n";

    } else {
        cout << "Unable to open file" << filename << "\n";
    }

}

double Structure::getH() const {
    return h;
}
