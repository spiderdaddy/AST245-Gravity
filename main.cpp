
#include <cstdlib>
#include <thread>
#include <cmath>

#include "graphics.h"

#include "DirectNBodyGravityCalculator.h"
#include "OctaTree.h"

void Initialize(int argc, char *argv[]) {


}

void calculateDirectNBodyGravityThread() {

    Structure *structure1 = new Structure("../data.ascii");
    setStructure(structure1);
    DirectNBodyGravityCalculator *calculator = new DirectNBodyGravityCalculator(structure1);

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

void testOctaTreeThread() {

    OctaTree * ot = new OctaTree(7, 1.0);
    ot->printNodes();

}

void testTreePopulation() {
    Structure *structure = new Structure("../data.ascii");
    std::vector<Structure::Position> pos = structure->getPositions();

    auto maxPos = structure->getMaxPosition();
    auto minPos = structure->getMinPosition();
    auto max_x = std::max(std::abs(maxPos.pos.x), std::abs(minPos.pos.x));
    auto max_y = std::max(std::abs(maxPos.pos.y), std::abs(minPos.pos.y));
    auto max_z = std::max(std::abs(maxPos.pos.z), std::abs(minPos.pos.z));

    auto h = std::ceil(std::max(std::max(max_x, max_y), max_z));

    OctaTree * ot = new OctaTree(7, h);

    auto startTime = std::chrono::system_clock::now();
    std::for_each(pos.begin(), pos.end(),
        [ot](Structure::Position p){
            auto otn = ot->getNode(p.pos.x,p.pos.y,p.pos.z);
            while (otn != nullptr) {
                otn->mass += 1.0;
                otn = otn->parent;
            }
        });
    auto endTime = std::chrono::system_clock::now();
    std::cout << "Tree population time:" << std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() << "\n";

    ot->printNodes();

}


int main(int argc, char *argv[]) {

    Initialize(argc, argv);

    std::thread t1(testTreePopulation);
    t1.join();

//    InitializeGraphics(argc, argv);
//    GraphicsMainLoop();

    exit(EXIT_SUCCESS);
}







