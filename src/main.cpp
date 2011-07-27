#include <iostream>
#include <fstream>
#include <cmath>
#include <GL/glut.h>

#include "model.hpp"

using namespace std;

float xrot = 0.0,
      yrot = 0.0,
      fov  = 30.0;

int bcull = 0;

Model model;

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov,1.,2.,100.);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();      
    gluLookAt(0.,0.,3.,0.,0.,0.,0.,1.,0.);
    if(bcull){
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
    } else 
        glDisable(GL_CULL_FACE);


    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glColor3f(1.,0.,0.);


    glRotatef(yrot,0.0f,1.0f,0.0f);  
    glRotatef(xrot,1.0f,0.0f,0.0f);

    model.display();

    glFlush();
    glutSwapBuffers();
}
void myKeys(unsigned char key, int x, int y){

    if(key=='b') bcull=(bcull+1)%2;

    glutPostRedisplay();
}
//----------------------------------------------------------------

void splFun(int k, int x, int y){
    switch(k){
    case GLUT_KEY_UP: fov-=2.0; break;
    case GLUT_KEY_DOWN: fov+=2.0; break;
    case GLUT_KEY_LEFT: yrot-=5.0; break;
    case GLUT_KEY_RIGHT: yrot+=5.0; break;
    case GLUT_KEY_PAGE_UP: xrot-=5.0; break;
    case GLUT_KEY_PAGE_DOWN: xrot+=5.0; break;

    }
    if(fov<2.0) fov=2.0; 
    glutPostRedisplay();
}

//----------------------------------------------------------------

void init() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);     
    glClearDepth(1.0f);      
    glEnable(GL_DEPTH_TEST);  
    glEnable(GL_NORMALIZE);
}

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB|GLUT_DEPTH);
   glutInitWindowSize (800, 800); 
   glutInitWindowPosition (10, 10);
   glutCreateWindow (argv[0]);

   model = Model(string("camel.off"));
   init();

   glutDisplayFunc(display); 
   glutSpecialFunc(splFun);
   glutKeyboardFunc(myKeys);
   glutMainLoop();
   return 0;
}
