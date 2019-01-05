
#include <cstdlib>
#include <thread>
#include <zconf.h>

#include "graphics.h"

#include "DirectNBodyGravityCalculator.h"

DirectNBodyGravityCalculator *calculator;
Structure *structure1;

void Initialize(int argc, char *argv[]) {

    structure1 = new Structure("../data.ascii");
    setStructure(structure1);
    calculator = new DirectNBodyGravityCalculator(structure1);

}

void calculationThread() {
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




int main(int argc, char *argv[]) {

    Initialize(argc, argv);

    std::thread t1(calculationThread);
    t1.join();

    InitializeGraphics(argc, argv);
    GraphicsMainLoop();

    exit(EXIT_SUCCESS);
}







