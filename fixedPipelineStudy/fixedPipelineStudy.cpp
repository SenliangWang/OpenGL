#include <iostream>
#include "GL/freeglut.h"

//Screen Constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_FPS = 60;

bool initGL();
/*
Pre Condition:
 -A valid OpenGL context
Post Condition:
 -Initializes matrices and clear color
 -Reports to console if there was an OpenGL error
 -Returns false if there was an error in initialization
Side Effects:
 -Projection matrix is set to identity matrix
 -Modelview matrix is set to identity matrix
 -Matrix mode is set to modelview
 -Clear color is set to black
*/

void update();
/*
Pre Condition:
 -None
Post Condition:
 -Does per frame logic
Side Effects:
 -None
*/

void render();
/*
Pre Condition:
 -A valid OpenGL context
 -Active modelview matrix
Post Condition:
 -Renders the scene
Side Effects:
 -Clears the color buffer
 -Swaps the front/back buffer
*/


void runMainLoop(int val);
/*
Pre Condition:
 -Initialized freeGLUT
Post Condition:
 -Calls the main loop functions and sets itself to be called back in 1000 / SCREEN_FPS milliseconds
Side Effects:
 -Sets glutTimerFunc
*/



bool initGL()
{
    //Initialize Projection Matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    //Initialize Modelview Matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //Initialize clear color
    glClearColor(0.f, 0.f, 0.f, 1.f);

    //Check for error
    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
    {
        printf("Error initializing OpenGL! %s\n", gluErrorString(error));
        return false;
    }

    return true;
}


void update()
{

}


void render()
{
    //Clear color buffer
    glClear(GL_COLOR_BUFFER_BIT);
    
    //Render quad
    glBegin(GL_QUADS);
    glVertex2f(-0.5f, -0.5f);
    glVertex2f(0.5f, -0.5f);
    glVertex2f(0.5f, 0.5f);
    glVertex2f(-0.5f, 0.5f);
    glEnd();

    //Update screen
    glutSwapBuffers();
}

int main(int argc, char* args[])
{
    //Initialize FreeGLUT
    glutInit(&argc, args);

    //Create OpenGL 2.1 context
    glutInitContextVersion(2, 1);


    //Create Double Buffered Window
    glutInitDisplayMode(GLUT_DOUBLE);
    glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    glutCreateWindow("OpenGL");


    //Do post window/context creation initialization
    if (!initGL())
    {
        printf("Unable to initialize graphics library!\n");
        return 1;
    }

    //Set rendering function
    glutDisplayFunc(render);

    //Set main loop
    glutTimerFunc(1000 / SCREEN_FPS, runMainLoop, 0);

    //Start GLUT main loop
    glutMainLoop();

    return 0;

}


void runMainLoop(int val)
{
    //Frame logic
    update();
    render();

    //Run frame one more time
    glutTimerFunc(1000 / SCREEN_FPS, runMainLoop, val);
}