//
// Created by lf on 08/12/18.
//

#include <iostream>
#include <cstdio>
#include <vector>
#include <cstring>
#include <thread>
#include <FreeImage.h>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "common/shader.hpp"

#include "graphics.h"

#define WINDOW_TITLE_PREFIX "Gravity"

using namespace glm;

int CurrentWidth = 800;
int CurrentHeight = 800;
int WindowHandle = 0;
unsigned FrameCount = 0;

GLuint VaoId;
GLuint VboAreaId;
GLuint VboPropertyId;

GLuint programID;
GLuint MatrixID;

glm::mat4 MVP;

Structure * structure;

bool render_on = true;

void pause_render() {render_on = false; }
void resume_render() {render_on = true; }

bool rendering = false;

bool isRendering() { return rendering ;}

void GraphicsMainLoop() {

    while (structure == nullptr) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    glutMainLoop();

}

void setStructure( Structure * s ) {
    structure = s;
    CreateVBO();
}

bool keyPressed = false;
void keyDownFunc( unsigned char key, int x, int y) {
    keyPressed = true;
}

void keyUpFunc( unsigned char key, int x, int y) {
    keyPressed = false;
}

bool getKeyPressed() {
    return keyPressed;
}

void InitializeGraphics(int argc, char *argv[]) {

    InitWindow(argc, argv);

    glewExperimental = GL_TRUE;
    GLenum GlewInitResult = glewInit();

    if (GLEW_OK != GlewInitResult) {
        fprintf(
                stderr,
                "ERROR: %s\n",
                glewGetErrorString(GlewInitResult)
        );
        exit(EXIT_FAILURE);
    }

    fprintf(
            stdout,
            "INFO: OpenGL Version: %s\n",
            glGetString(GL_VERSION)
    );

    CreateShaders();


    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    glutKeyboardFunc(keyDownFunc);
    glutKeyboardUpFunc(keyUpFunc);

}


void RenderFunction() {

    if ( ( structure == nullptr ) || !render_on ) {
        return;
    }

    ++FrameCount;

    rendering = true;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use our shader
    glUseProgram(programID);

    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

    std::vector<Structure::ObjectPositions> positions = structure->getObjectPositions();
    glBindBuffer(GL_ARRAY_BUFFER, VboAreaId);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(Structure::ObjectPositions), &positions[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Structure::XYZW_GL), 0);
    glEnableVertexAttribArray(1);

    structure->MapObjectToColor();
    std::vector<Structure::ObjectColours> colours = structure->getObjectColours();
    glBindBuffer(GL_ARRAY_BUFFER, VboPropertyId);
    glBufferData(GL_ARRAY_BUFFER, colours.size() * sizeof(Structure::ObjectColours), &colours[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Structure::RGBA_GL), 0);
    glEnableVertexAttribArray(0);

    glDrawArrays(GL_POINTS, 0, (GLsizei)positions.size());

    glFlush();
    glutSwapBuffers();
    rendering = false;
}


void IdleFunction() {
    glutPostRedisplay();
}

void TimerFunction(int Value) {
    if (0 != Value) {
        char *TempString = (char *)
                malloc(512 + strlen(WINDOW_TITLE_PREFIX));

        sprintf(
                TempString,
                "%s: %d Frames Per Second @ %d x %d",
                WINDOW_TITLE_PREFIX,
                FrameCount * 4,
                CurrentWidth,
                CurrentHeight
        );

        glutSetWindowTitle(TempString);
        free(TempString);
    }

    FrameCount = 0;
    glutTimerFunc(250, TimerFunction, 1);
}

void Cleanup() {
    DestroyShaders();
    DestroyVBO();
}

void CreateVBO() {
    unsigned int ErrorCheckValue = glGetError();

    glGenVertexArrays(1, &VaoId);
    glBindVertexArray(VaoId);

    std::vector<Structure::ObjectPositions> positions = structure->getObjectPositions();

    glGenBuffers(1, &VboAreaId);
    glBindBuffer(GL_ARRAY_BUFFER, VboAreaId);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(Structure::ObjectPositions), &positions[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Structure::XYZW_GL), 0);
    glEnableVertexAttribArray(1);

    std::vector<Structure::ObjectColours> colours = structure->getObjectColours();

    glGenBuffers(1, &VboPropertyId);
    glBindBuffer(GL_ARRAY_BUFFER, VboPropertyId);
    glBufferData(GL_ARRAY_BUFFER, colours.size() * sizeof(Structure::ObjectColours), &colours[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Structure::RGBA_GL), 0);
    glEnableVertexAttribArray(0);

    ErrorCheckValue = glGetError();
    if (ErrorCheckValue != GL_NO_ERROR) {
        fprintf(
                stderr,
                "ERROR: Could not create a VBO: %s \n",
                gluErrorString(ErrorCheckValue)
        );

        exit(-1);
    }
}

void DestroyVBO() {
    unsigned int ErrorCheckValue = glGetError();

    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &VboPropertyId);

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &VaoId);

    ErrorCheckValue = glGetError();
    if (ErrorCheckValue != GL_NO_ERROR) {
        fprintf(
                stderr,
                "ERROR: Could not destroy the VBO: %s \n",
                gluErrorString(ErrorCheckValue)
        );

        exit(-1);
    }
}

void CreateShaders() {
    // Create and compile our GLSL program from the shaders
    programID = LoadShaders("../SimpleTransform.vertexshader",
                            "../SingleColor.fragmentshader");

    // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    // glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
    // Or, for an ortho camera :
    float xortho = 1;
    float yortho = 1;
    float factor = 1.1f;

    mat4x4 Projection = glm::ortho(-1.0f, -1.0f, -2.0f, 2.0f); // In world coordinates
    fprintf(
            stderr,
            "INFO: xortho: %f yortho: %f\n",
            xortho, yortho
    );

    // Camera matrix
    mat4x4 View = glm::lookAt(
            highp_vec3(0, 0, 1), // Camera in World Space
            highp_vec3(0, 0, 0), // and looks at the origin
            highp_vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
    );

    // Model matrix : an identity matrix (model will be at the origin)
    mat4x4 Model = mat4x4(1.0f);

    // Our ModelViewProjection : multiplication of our 3 matrices
    MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around

    // Get a handle for our "MVP" uniform
    MatrixID = glGetUniformLocation(programID, "MVP");


}

void DestroyShaders() {
    glDeleteProgram(programID);
}

void InitWindow(int argc, char *argv[]) {
    glutInit(&argc, argv);

    glutInitContextVersion(4, 0);
    glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
    glutInitContextProfile(GLUT_CORE_PROFILE);

    glutSetOption(
            GLUT_ACTION_ON_WINDOW_CLOSE,
            GLUT_ACTION_GLUTMAINLOOP_RETURNS
    );

    glutInitWindowSize(CurrentWidth, CurrentHeight);

    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA| GLUT_MULTISAMPLE);

    WindowHandle = glutCreateWindow(WINDOW_TITLE_PREFIX);

    if (WindowHandle < 1) {
        fprintf(
                stderr,
                "ERROR: Could not create a new rendering window.\n"
        );
        exit(EXIT_FAILURE);
    }

    glutReshapeFunc(ResizeFunction);
    glutDisplayFunc(RenderFunction);
    glutIdleFunc(IdleFunction);
    glutTimerFunc(0, TimerFunction, 0);
    glutCloseFunc(Cleanup);
}

void ResizeFunction(int Width, int Height) {
    CurrentWidth = Width;
    CurrentHeight = Height;
    //glViewport(0, 0, std::min(CurrentWidth, CurrentHeight), std::min(CurrentWidth, CurrentHeight));
    glViewport(0, 0, CurrentWidth, CurrentHeight);

}

void saveImage( string filename_base ) {

    RenderFunction();

    string filename = filename_base.append(".png");
    int width = 800;
    int height = 800;
    BYTE pixels [3*width*height];

    glReadPixels(900,0,width,height, GL_BGR, GL_UNSIGNED_BYTE, pixels);

    auto ErrorCheckValue = glGetError();
    if (ErrorCheckValue != GL_NO_ERROR) {
        fprintf(
                stderr,
                "ERROR: glReadPixels: %s \n",
                gluErrorString(ErrorCheckValue)
        );
    }

    FIBITMAP* Image = FreeImage_ConvertFromRawBits(pixels, width, height, 3*width, 24, 0xFF0000, 0x00FF00, 0x0000FF, false);
    FreeImage_Save(FIF_PNG, Image, &filename[0], 0);
}

