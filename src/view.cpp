#include "view.hpp"

#include <GL/glut.h>

static Model  view_model;
static double view_fov,
              view_near,
              view_far,
              view_aspect;

static int view_height,
           view_width,
           view_bcull;

int get_view_height() { return view_height; }
int get_view_width() { return view_width; }

void view_init(int argc, char *argv[]) {
    view_model = Model("models/camel.off");
    view_fov = 50.0;
    view_near = 1.0;
    view_far = 1000.0;
    view_aspect = 0.0;
    view_height = 0;
    view_width = 0;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(311, 496);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Assignment 1, Wim Looman");
    glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);
    glutIgnoreKeyRepeat(1);

    glClearColor( 0.95, 0.95, 0.95, 1.0 );
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
}

void view_reshape(int new_width, int new_height) {
    glViewport(0, 0, new_width, new_height);
    view_width = new_width;
    view_height = new_height;
    view_aspect = new_width / (double)new_height;
}

void view_display() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(view_fov, view_aspect, view_near, view_far);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    player_look();

    if (view_bcull) {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
    } else {
        glDisable(GL_CULL_FACE);
    }


    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glColor3f(1.0, 0.0, 0.0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
    view_model.display();

    glFlush();
    glutSwapBuffers();
}
void view_toggle_bcull() {
    view_bcull = (view_bcull+1)%2;
}
