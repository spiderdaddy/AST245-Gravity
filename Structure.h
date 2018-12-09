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

    struct XYZW_GL {
        float x, y, z, w;
    };

    struct RGBA_GL {
        float r, g, b, a;
    };

    struct ObjectPositions {
        XYZW_GL pos;
    };

    struct ObjectColours {
        RGBA_GL color;
    };

    void PrintPoints();

    void MapObjectToColor();

    void CalcSystemEnergy();

    std::vector<ObjectPositions> getObjectPositions();

    std::vector<ObjectColours> getObjectColours();

    void loadFile(std::string filename);

private:
    unsigned int num_objects;
    std::vector<Structure::ObjectPositions> positions;
    std::vector<Structure::ObjectColours> colours;

private:

};


#endif //GRAVITY_STRUCTURE_H
