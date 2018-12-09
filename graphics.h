//
// Created by lf on 08/12/18.
//

#ifndef GRAVITY_GRAPHICS_H
#define GRAVITY_GRAPHICS_H

#include <cstring>
#include "Structure.h"

using namespace std;

void RenderFunction();

void IdleFunction();

void TimerFunction(int);

void Cleanup();

void CreateVBO();

void DestroyVBO();

void CreateShaders();

void DestroyShaders();

void InitWindow(int argc, char *argv[]);

void ResizeFunction(int, int);

void InitializeGraphics(int argc, char *argv[]);

bool getKeyPressed();

void setStructure( Structure * s );

void GraphicsMainLoop();

void saveImage( string filename );

void pause_render();
void resume_render();

bool isRendering();

#endif //GRAVITY_GRAPHICS_H
