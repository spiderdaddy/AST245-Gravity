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
#include <GLFW/glfw3.h>
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
unsigned Frame = 0;

GLuint VaoId;
GLuint VboAreaId;
GLuint VboPropertyId;

GLuint programID;
GLuint matrixID;
GLuint cameraPositionID;

glm::mat4 MVP;

Structure * structure;
void setStructure( Structure * s ) {
    structure = s;
}

GravityCalculator * gravityCalculator;
void setCalculator( GravityCalculator * gc ) {
    gravityCalculator = gc;
}


bool render_on = true;

void pause_render() {render_on = false; }
void resume_render() {render_on = true; }

bool rendering = false;

bool isRendering() { return rendering ;}

void GraphicsMainLoop() {

    while (structure == nullptr
           || !structure->isReady() ) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    CreateVBO();

    glutMainLoop();

}

float radius = 100.0f;
float camTheta = -glm::quarter_pi<float>()/2.0f;
float camPhi = -glm::quarter_pi<float>();
float camRadius = radius;
float zoomFactor = 1.0f;


bool showAxis = true;
GLuint VaoAxisId;
GLuint VboAxisLinesId;
GLuint VboAxisColorsId;

std::vector<Structure::XYZW_GL> axis_pos{
    {0.0f,        0.0f,    0.0f,    1.0f},
    {camRadius/2.0f, 0.0f,    0.0f,    1.0f},
    {0.0f,        0.0f,    0.0f,    1.0f},
    {0.0f,        camRadius/2.0f,  0.0f,    1.0f},
    {0.0f,        0.0f,    0.0f,    1.0f},
    {0.0f,        0.0f,    camRadius/2.0f,  1.0f}
};

std::vector<Structure::RGBA_GL> axis_colour {
    {0.0f,    0.0f,    1.0f,    0.0f},
    {0.0f,    0.0f,    1.0f,    0.0f},
    {0.0f,    1.0f,    0.0f,    0.0f},
    {0.0f,    1.0f,    0.0f,    0.0f},
    {1.0f,    0.0f,    0.0f,    0.0f},
    {1.0f,    0.0f,    0.0f,    0.0f},
};

bool showTreeFrame = false;
bool treeFrameLevelOnly = false;
int treeFrameLevel = 8;
GLuint VaoTreeFrameId;
GLuint VboTreeFrameLinesId;
GLuint VboTreeFrameColorsId;
std::vector<Structure::XYZW_GL> tree_frame_pos;
std::vector<Structure::RGBA_GL> tree_frame_colour;

bool keyPressed = false;
void keyDownFunc( unsigned char key, int x, int y) {

    keyPressed = true;
    float cameraSpeed = glm::pi<float>()/90.0f; // adjust accordingly
    switch (key) {
        case 'w':
            camRadius -= 0.5f;
            break;
        case 's':
            camRadius += 0.5f;
            break;
        case 'j':
            camTheta -= cameraSpeed;
            break;
        case 'l':
            camTheta += cameraSpeed;
            break;
        case 'i':
            camPhi += cameraSpeed;
            break;
        case 'k':
            camPhi -= cameraSpeed;
            break;
        case 'q':
            zoomFactor *= 1.1;
            //camZ -= cameraSpeed;
            break;
        case 'a':
            zoomFactor /= 1.1;
            //camZ += cameraSpeed;
            break;
        case 'c':
            camTheta = -glm::quarter_pi<float>()/2.0f;
            camPhi = -glm::quarter_pi<float>();
            camRadius = radius;
            zoomFactor=1.0f;
            treeFrameLevel = 9;
            break;

        case 'x':
            showAxis = !showAxis;
            break;

        case 'b':
            showTreeFrame = !showTreeFrame;
            break;

        case 'r':
            treeFrameLevel++;
            cout << "Level: " << treeFrameLevel << "\n";
            break;
        case 'f':
            treeFrameLevel--;
            if (treeFrameLevel < 0 ) {
                treeFrameLevel = 0;
            }
            cout << "Level: " << treeFrameLevel << "\n";
            break;

        case 'v':
            treeFrameLevelOnly = !treeFrameLevelOnly;
            break;


    }

    if (camRadius < 1.0f) { camRadius = 0.5f; }
    if (camPhi > 0.0f ) { camPhi = 0.0f; }
    if (camPhi < -glm::pi<float>()) {camPhi = -glm::pi<float>();}
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

Structure::XYZW_GL c000 = {0.0f, 0.0f, 0.0f, 1.0f};
Structure::XYZW_GL c100 = {0.0f, 0.0f, 0.0f, 1.0f};
Structure::XYZW_GL c010 = {0.0f, 0.0f, 0.0f, 1.0f};
Structure::XYZW_GL c110 = {0.0f, 0.0f, 0.0f, 1.0f};
Structure::XYZW_GL c001 = {0.0f, 0.0f, 0.0f, 1.0f};
Structure::XYZW_GL c101 = {0.0f, 0.0f, 0.0f, 1.0f};
Structure::XYZW_GL c011 = {0.0f, 0.0f, 0.0f, 1.0f};
Structure::XYZW_GL c111 = {0.0f, 0.0f, 0.0f, 1.0f};

Structure::RGBA_GL color = {1.0f, 0.5f, 0.0f, 0.0f};

bool drawTreeFrameNode(OctaTreeNode *n) {

    c000.x = (float) n->xmin;
    c000.y = (float) n->ymin;
    c000.z = (float) n->zmin;
    c100.x = (float) n->xmax;
    c100.y = (float) n->ymin;
    c100.z = (float) n->zmin;
    c010.x = (float) n->xmin;
    c010.y = (float) n->ymax;
    c010.z = (float) n->zmin;
    c110.x = (float) n->xmax;
    c110.y = (float) n->ymax;
    c110.z = (float) n->zmin;
    c001.x = (float) n->xmin;
    c001.y = (float) n->ymin;
    c001.z = (float) n->zmax;
    c101.x = (float) n->xmax;
    c101.y = (float) n->ymin;
    c101.z = (float) n->zmax;
    c011.x = (float) n->xmin;
    c011.y = (float) n->ymax;
    c011.z = (float) n->zmax;
    c111.x = (float) n->xmax;
    c111.y = (float) n->ymax;
    c111.z = (float) n->zmax;

    // Front
    tree_frame_pos.push_back(c000);
    tree_frame_colour.push_back(color);
    tree_frame_pos.push_back(c100);
    tree_frame_colour.push_back(color);

    tree_frame_pos.push_back(c000);
    tree_frame_colour.push_back(color);
    tree_frame_pos.push_back(c010);
    tree_frame_colour.push_back(color);

    tree_frame_pos.push_back(c010);
    tree_frame_colour.push_back(color);
    tree_frame_pos.push_back(c110);
    tree_frame_colour.push_back(color);

    tree_frame_pos.push_back(c100);
    tree_frame_colour.push_back(color);
    tree_frame_pos.push_back(c110);
    tree_frame_colour.push_back(color);

    // Middle
    tree_frame_pos.push_back(c000);
    tree_frame_colour.push_back(color);
    tree_frame_pos.push_back(c001);
    tree_frame_colour.push_back(color);

    tree_frame_pos.push_back(c100);
    tree_frame_colour.push_back(color);
    tree_frame_pos.push_back(c101);
    tree_frame_colour.push_back(color);

    tree_frame_pos.push_back(c010);
    tree_frame_colour.push_back(color);
    tree_frame_pos.push_back(c011);
    tree_frame_colour.push_back(color);

    tree_frame_pos.push_back(c110);
    tree_frame_colour.push_back(color);
    tree_frame_pos.push_back(c111);
    tree_frame_colour.push_back(color);

    // Back
    tree_frame_pos.push_back(c001);
    tree_frame_colour.push_back(color);
    tree_frame_pos.push_back(c101);
    tree_frame_colour.push_back(color);

    tree_frame_pos.push_back(c001);
    tree_frame_colour.push_back(color);
    tree_frame_pos.push_back(c011);
    tree_frame_colour.push_back(color);

    tree_frame_pos.push_back(c101);
    tree_frame_colour.push_back(color);
    tree_frame_pos.push_back(c111);
    tree_frame_colour.push_back(color);

    tree_frame_pos.push_back(c011);
    tree_frame_colour.push_back(color);
    tree_frame_pos.push_back(c111);
    tree_frame_colour.push_back(color);

}

void drawTreeFrameNodeRecursive(OctaTreeNode *node) {
    if (node != nullptr and node->level <= treeFrameLevel) {
        for (int i = 0; i < 8; i++) {
            if (node->leaf[i] != nullptr) {
                drawTreeFrameNodeRecursive(node->leaf[i]);
            }
        }
        if ( treeFrameLevelOnly ) {
            if (node->level == treeFrameLevel ) {
                drawTreeFrameNode(node);
            }
        } else {
            drawTreeFrameNode(node);
        }
    }
}


void caculateTreeFrame() {

    if (gravityCalculator == nullptr
        || gravityCalculator->getOctaTree() == nullptr
        || gravityCalculator->getBuildInProgress() ) {
        return;
    }

    tree_frame_pos.clear();
    tree_frame_colour.clear();

    OctaTreeNode *n = gravityCalculator->getOctaTree()->getHead();

    drawTreeFrameNodeRecursive(n);

}

void RenderFunction() {

    if ( ( structure == nullptr ) || !render_on ) {
        return;
    }

    ++FrameCount;

    rendering = true;

    //cout << camTheta << " " << camPhi << " " << zoomFactor << "\n";

    //glm::vec3 cameraPos = glm::vec3(camRadius*sin(camPhi)*cos(camTheta),
    //                                camRadius*sin(camPhi)*sin(camTheta),
    //                                camRadius*cos(camPhi) );


    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, camRadius);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);


    glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, cameraUp);



    // Model matrix : an identity matrix (model will be at the origin)
    mat4x4 Model = mat4x4(1.0f);
    Model = glm::rotate(Model, camPhi, glm::vec3(1.0f, 0.0f, 0.0f));
    Model = glm::rotate(Model, camTheta, glm::vec3(0.0f, 0.0f, 1.0f));
    Model = glm::scale(Model, glm::vec3(zoomFactor));

    // mat4x4 Projection = glm::ortho(-10.0f, -1.0f, -2.0f, 2.0f); // In world coordinates
    //glm::mat4 Projection = glm::ortho(-radius,radius,-radius,radius,-radius*zoomFactor,2.0f*radius*zoomFactor); // In world coordinates
    glm::mat4 Projection = glm::perspective(glm::radians(45.0f), (float)CurrentWidth / (float)CurrentHeight, 0.0f, 2.0f*radius*zoomFactor);


    // Our ModelViewProjection : multiplication of our 3 matrices
    MVP = Projection * view * Model; // Remember, matrix multiplication is the other way around


    // Get a handle for our "MVP" uniform
    matrixID = glGetUniformLocation(programID, "MVP");
    glUniformMatrix4fv(matrixID, 1, GL_FALSE, &MVP[0][0]);

    // Get a handle for our "cameraPosition" uniform
    //cameraPositionID = glGetUniformLocation(programID, "cameraPosition");
    //glUniform3fv(cameraPositionID, 1, &view[0]);


    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use our shader
    glUseProgram(programID);

    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
    glUniformMatrix4fv(matrixID, 1, GL_FALSE, &MVP[0][0]);

    std::vector<Structure::Position> positions = structure->getPositions();
    glBindBuffer(GL_ARRAY_BUFFER, VboAreaId);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(Structure::Position), &positions[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Structure::XYZW_GL), 0);
    glEnableVertexAttribArray(1);

    structure->MapObjectToColor();
    std::vector<Structure::Colour> colours = structure->getColours();
    glBindBuffer(GL_ARRAY_BUFFER, VboPropertyId);
    glBufferData(GL_ARRAY_BUFFER, colours.size() * sizeof(Structure::Colour), &colours[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Structure::RGBA_GL), 0);
    glEnableVertexAttribArray(0);

    glDrawArrays(GL_POINTS, 0, (GLsizei)positions.size());

    if (showAxis) {

        axis_pos[1].x = camRadius / 5.0f;
        axis_pos[3].y = camRadius / 5.0f;
        axis_pos[5].z = camRadius / 5.0f;
        glBindBuffer(GL_ARRAY_BUFFER, VboAxisLinesId);
        glBufferData(GL_ARRAY_BUFFER, axis_pos.size() * sizeof(Structure::XYZW_GL), &axis_pos[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Structure::XYZW_GL), 0);
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, VboAxisColorsId);
        glBufferData(GL_ARRAY_BUFFER, axis_colour.size() * sizeof(Structure::RGBA_GL), &axis_colour[0], GL_STATIC_DRAW);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Structure::RGBA_GL), 0);
        glEnableVertexAttribArray(0);

        glDrawArrays(GL_LINES, 0, 6);
    }



    if (showTreeFrame)  {

        caculateTreeFrame();

        if ( tree_frame_pos.size() > 0) {

            glBindBuffer(GL_ARRAY_BUFFER, VboTreeFrameLinesId);
            glBufferData(GL_ARRAY_BUFFER, tree_frame_pos.size() * sizeof(Structure::XYZW_GL), tree_frame_pos.data(),
                         GL_STATIC_DRAW);
            glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Structure::XYZW_GL), 0);
            glEnableVertexAttribArray(1);

            glBindBuffer(GL_ARRAY_BUFFER, VboTreeFrameColorsId);
            glBufferData(GL_ARRAY_BUFFER, tree_frame_colour.size() * sizeof(Structure::RGBA_GL), tree_frame_colour.data(),
                         GL_STATIC_DRAW);
            glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Structure::RGBA_GL), 0);
            glEnableVertexAttribArray(0);

            glDrawArrays(GL_LINES, 0, (GLsizei) tree_frame_pos.size());
        }
    }



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
    Frame++;
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

    std::vector<Structure::Position> positions = structure->getPositions();

    glGenBuffers(1, &VboAreaId);
    glBindBuffer(GL_ARRAY_BUFFER, VboAreaId);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(Structure::Position), &positions[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Structure::XYZW_GL), 0);
    glEnableVertexAttribArray(1);

    std::vector<Structure::Colour> colours = structure->getColours();

    glGenBuffers(1, &VboPropertyId);
    glBindBuffer(GL_ARRAY_BUFFER, VboPropertyId);
    glBufferData(GL_ARRAY_BUFFER, colours.size() * sizeof(Structure::Colour), &colours[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Structure::RGBA_GL), 0);
    glEnableVertexAttribArray(0);

    // Buffers for the Axis

    glGenVertexArrays(1, &VaoAxisId);
    glBindVertexArray(VaoAxisId);

    glGenBuffers(1, &VboAxisLinesId);
    glGenBuffers(1, &VboAxisColorsId);

    // Buffers for the Axis

    glGenVertexArrays(1, &VaoTreeFrameId);
    glBindVertexArray(VaoTreeFrameId);

    glGenBuffers(1, &VboTreeFrameLinesId);
    glGenBuffers(1, &VboTreeFrameColorsId);



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
    matrixID = glGetUniformLocation(programID, "MVP");
    glUniformMatrix4fv(matrixID, 1, GL_FALSE, &MVP[0][0]);

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

