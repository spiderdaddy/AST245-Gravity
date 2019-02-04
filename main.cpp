
#include <cstdlib>
#include <thread>
#include <cmath>

#include "graphics.h"

#include "OctaTree.h"

#include "DirectNBodyGravityCalculator.h"
#include "OctaTreeMultipoleGravityCalculator.h"


void Initialize(int argc, char *argv[]) {


}

void TimeIntegrateOctaTreeMultipoleGravityThread() {

    Structure *structure = new Structure("../data.ascii");
    setStructure(structure);
    auto calculator = new OctaTreeMultipoleGravityCalculator(structure);
    setCalculator(calculator);


    auto theta = 0.5;

    calculator->setMaxOpeningAngle(theta);
    std::vector<Structure::Acceleration> &acc = structure->getAccelerations();
    std::vector<Structure::Position> &pos = structure->getPositions();
    std::vector<Structure::Velocity> &v = structure->getVelocities();

    double dt = 0.001;
    double dt_2 = dt / 2;

    for (int i = 0; i < 2000; i++) {
        std::this_thread::yield();

        double px = 0;
        double py = 0;
        double pz = 0;
        double pe = 0;
        double ke = 0;

        // leapfrog method

        for( unsigned j = 0; j < acc.size(); j++) {
            pos[j].pos.x += v[j].v.x * dt_2;
            pos[j].pos.y += v[j].v.y * dt_2;
            pos[j].pos.z += v[j].v.z * dt_2;
        }

        calculator->calculate();
        cout << "Calculation time = " << calculator->getTime() << "ms \n";
        if( i == 0 ) {
            string filename;
            filename.append("OctaTreeMultipole-");
            filename.append(std::to_string(theta));

            structure->saveFile(filename);
        }

        for( unsigned j = 0; j < acc.size(); j++) {
            v[j].v.x += acc[j].a.x * dt;
            v[j].v.y += acc[j].a.y * dt;
            v[j].v.z += acc[j].a.z * dt;

            pos[j].pos.x += v[j].v.x * dt_2;
            pos[j].pos.y += v[j].v.y * dt_2;
            pos[j].pos.z += v[j].v.z * dt_2;

            ke += (v[j].v.x*v[j].v.x + v[j].v.y*v[j].v.y + v[j].v.z*v[j].v.z)/2.0;
            pe += std::sqrt(pos[j].pos.x*pos[j].pos.x+pos[j].pos.y*pos[j].pos.y+pos[j].pos.z*pos[j].pos.z);
            px += v[j].v.x;
            py += v[j].v.y;
            pz += v[j].v.z;

        }

        cout << i << "(" << px << ", " << py << ", " << pz << ") ";
        cout << "(" << ke << ", " << pe << ", " << ke + pe << ")\n";
    }



}

void calculateOctaTreeMultipoleGravityThread() {

    Structure *structure = new Structure("../data.ascii");
    setStructure(structure);
    auto calculator = new OctaTreeMultipoleGravityCalculator(structure);
    double theta[] = {0.05, 0.1, 0.2, 0.4, 0.8, 1.0};

    for ( int i = 0; i < sizeof(theta)/sizeof(theta[0]) ; i++ ) {
        cout << "Max opening angle = " << theta[i] << "\n";
        calculator->setMaxOpeningAngle(theta[i]);
        calculator->calculate();
        cout << "Calculation time = " << calculator->getTime() << "ms \n";
        string filename;
        filename.append("OctaTreeMultipole-");
        filename.append(std::to_string(theta[i]));

        structure->saveFile(filename);
    }

}

void calculateDirectNBodyGravityThread() {

    Structure *structure1 = new Structure("../data.ascii");
    setStructure(structure1);
    auto calculator = new DirectNBodyGravityCalculator(structure1);

    float w0 = structure1->meanInterParticleSeparation();

    cout << "meanInterParticleSeparation = " << w0 << "\n";

    int i_max = int(w0)*2;

    for ( int i = 0; i < i_max; i ++) {
        float w = ((float)(i))/i_max * 2 * w0;
        cout << "InterParticleSeparation = " << w << "\n";
        calculator->setSofteningEpsilon(w);
        calculator->calculate();
        cout << "Calculation time = " << calculator->getTime() << "ms \n";
        string filename;
        filename.append("DirectNBody-");
        filename.append(std::to_string(w));

        structure1->saveFile(filename);
    }

}

void testTreePopulation() {
    Structure *structure = new Structure("../data.ascii");
    std::vector<Structure::Position> pos = structure->getPositions();


    auto startTime = std::chrono::system_clock::now();

    auto ot = new OctaTree(structure);

    auto endTime = std::chrono::system_clock::now();
    std::cout << "Tree population time:" << std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() << "\n";

    ot->printNodes();

}


int main(int argc, char *argv[]) {

    Initialize(argc, argv);

    std::thread t1(TimeIntegrateOctaTreeMultipoleGravityThread);
    t1.detach();

    /*for (int i = 0; i < 200000; i++) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }*/

    InitializeGraphics(argc, argv);
    GraphicsMainLoop();

    exit(EXIT_SUCCESS);
}







