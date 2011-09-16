#include "view.hpp"

#include <GL/glut.h>

#include "player.hpp"
#include "model.hpp"

static double view_fov,
              view_near,
              view_far,
              view_aspect;

static int view_height,
           view_width;

static bool view_bcull, view_wireframe, view_orig_wireframe, view_original;

static Model *view_model;
static Model original_model;

int get_view_height() { return view_height; }
int get_view_width() { return view_width; }

void view_init(Model *model, int argc, char *argv[]) {
    view_model = model;
    original_model = *model;

    view_fov = 50.0;
    view_near = 0.1;
    view_far = 100.0;

    view_bcull = false;

    view_aspect = 0.0;
    view_height = 0;
    view_width = 0;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(10, 10);
    glutCreateWindow("Assignment 1, Wim Looman");

    glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);
    glutIgnoreKeyRepeat(1);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);         
    glClearDepth(1.0f);          
    glEnable(GL_DEPTH_TEST);  
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHTING);
    glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE ) ;
    glEnable ( GL_COLOR_MATERIAL ) ;
    float params[4] = {0.0, 0.0, 0.0, 1.0};
    glLightfv(GL_LIGHT0, GL_AMBIENT, params);
    params[0] = params[1] = params[2] = 1.0;
    glLightfv(GL_LIGHT0, GL_DIFFUSE, params);
    glLightfv(GL_LIGHT0, GL_SPECULAR, params);
    glEnable(GL_LIGHT0);
    GLfloat params1[4];
    params1[0] = params1[1] = params1[2] = 0.0;
    params1[4] = 1.0;
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, params1);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, params1);
}

void view_reshape(int new_width, int new_height) {
    glViewport(0, 0, new_width, new_height);
    view_width = new_width;
    view_height = new_height;
    view_aspect = new_width / (double)new_height;
}

void view_display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(view_fov, view_aspect, view_near, view_far);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    player_look();
    //gluLookAt(0.,0.,3.,0.,0.,0.,0.,1.,0.);

    if (view_bcull) {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
    } else {
        glDisable(GL_CULL_FACE);
    }


    if (view_wireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDisable(GL_LIGHTING);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glEnable(GL_LIGHTING);
    }

    glColor3f(0.8,0.0,0.0);
    view_model->display();

    if (view_original) {
        if (view_orig_wireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDisable(GL_LIGHTING);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glEnable(GL_LIGHTING);
        }

        glColor3f(0.0,0.8,0.0);
        original_model.display();
    }

    glFlush();
    glutSwapBuffers();
}

void view_toggle_bcull() {
    view_bcull = !view_bcull;
}

void view_toggle_orig_wireframe() {
    view_orig_wireframe = !view_orig_wireframe;
}

void view_toggle_wireframe() {
    view_wireframe = !view_wireframe;
}

void view_toggle_original() {
    view_original = !view_original;
}

void view_reset() {
    *view_model = original_model;
}
