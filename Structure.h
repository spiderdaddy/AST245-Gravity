//
// Created by lf on 09/12/18.
//

#ifndef GRAVITY_STRUCTURE_H
#define GRAVITY_STRUCTURE_H


#include <string>
#include <vector>

#define G 1.0

class Structure {

public:

    Structure(std::string filename );

    struct XYZ {
        double x, y, z;
    };

    struct XYZW_GL {
        float x, y, z, w;
    };

    struct RGBA_GL {
        float r, g, b, a;
    };

    struct Mass {
        double m;
    };

    struct Position {
        XYZW_GL pos;
    };

    struct Colour {
        RGBA_GL color;
    };

    struct Velocity {
        XYZ v;
    };

    struct Acceleration {
        XYZ a;
    };

    struct Softening {
        float s;
    };

    struct Potential {
        float pot;
    };

    void PrintPoints();

    void MapObjectToColor();

    void CalcSystemEnergy();

    void loadFile(std::string filename);
    void saveFile(std::string filename);

    std::vector<Position> &getPositions();

    std::vector<Colour> &getColours();

    std::vector<Acceleration> &getAccelerations();

    std::vector<Mass> &getMasses();

    std::vector<Velocity> &getVelocities();

    std::vector<Softening> &getSoftenings();

    std::vector<Potential> &getPotentials();

    float meanInterParticleSeparation();

    const Position &getMinPosition() const;

    const Position &getMaxPosition() const;

private:
    unsigned int num_objects;

    std::vector<Structure::Mass> masses;

    double h;

    bool ready = false;
public:
    double getH() const;
    bool isReady() { return ready; }

private:
    std::vector<Structure::Velocity> velocities;
    std::vector<Structure::Softening> softenings;
    std::vector<Structure::Potential> potentials;
    std::vector<Structure::Position> positions;
    std::vector<Structure::Colour> colours;
    std::vector<Structure::Acceleration> accelerations;

    Structure::Position minPosition;
    Structure::Position maxPosition;

};


#endif //GRAVITY_STRUCTURE_H
