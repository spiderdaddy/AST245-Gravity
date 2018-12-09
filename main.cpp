
#include <cstdlib>
#include <thread>
#include <zconf.h>

#include "graphics.h"
#include "Structure.h"

void Initialize(int argc, char *argv[]) {

}


int main(int argc, char *argv[]) {

    Initialize(argc, argv);
    InitializeGraphics(argc, argv);

    Structure * s = new Structure("");
    setStructure(s);

    GraphicsMainLoop();

    exit(EXIT_SUCCESS);
}







