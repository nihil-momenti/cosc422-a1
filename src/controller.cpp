#include "controller.hpp"

#include <algorithm>

#include "view.hpp"
#include "player.hpp"
#include "model.hpp"

static std::pair<double,double> mouse_pos;
static int left_button_state;

static void controller_display();
static void controller_reshape(int,int);
static void controller_mouseButtonFunc(int,int,int,int);
static void controller_mouseMotionFunc(int,int);
static void controller_keyboardFunc(unsigned char,int,int);
static void controller_keyboardUpFunc(unsigned char,int,int);

static Model *controller_model;

void controller_init(Model* model, int argc, char *argv[]) {
    controller_model = model;

    player_init();
    view_init(model, argc, argv);
    mouse_pos = std::pair<double,double>(0,0);
    left_button_state = GLUT_UP;

    glutDisplayFunc(controller_display);
    glutReshapeFunc(controller_reshape);
    glutMouseFunc(controller_mouseButtonFunc);
    glutMotionFunc(controller_mouseMotionFunc);
    glutKeyboardFunc(controller_keyboardFunc);
    glutKeyboardUpFunc(controller_keyboardUpFunc);
}

void controller_display() {
    view_display();
}

void controller_reshape(int a, int b) {
    view_reshape(a, b);
}

void controller_mouseButtonFunc(int button, int state, int x, int y) {
    static std::pair<double,double> mouse_initial_pos;
    if (button == GLUT_LEFT_BUTTON) {
        mouse_pos = std::pair<int,int>(x, y);
        left_button_state = state;
        if (state == GLUT_DOWN) {
            mouse_initial_pos = std::pair<int,int>(x, y);
            glutSetCursor(GLUT_CURSOR_NONE);
        } else {
            glutWarpPointer(mouse_initial_pos.first, mouse_initial_pos.second);
            glutSetCursor(GLUT_CURSOR_INHERIT);
        }
    }
}

void controller_mouseMotionFunc(int x, int y) {
    if (left_button_state == GLUT_DOWN) {
        std::pair<double,double> rotation = std::pair<double,double>(
            (x - mouse_pos.first) / (double) get_view_width(),
            (y - mouse_pos.second) / (double) get_view_height()
        );
        
        player_turn(rotation);
        
        if (x < 0) {
            x = get_view_width();
            glutWarpPointer(x, y);
        } else if (x > get_view_width()) {
            x = 0;
            glutWarpPointer(x, y);
        }

        if (y < 0) {
            y = get_view_height();
            glutWarpPointer(x, y);
        } else if (y > get_view_height()) {
            y = 0;
            glutWarpPointer(x, y);
        }
    }

    mouse_pos = std::pair<int, int>(x, y);
    glutPostRedisplay();
}

void controller_keyboardFunc(unsigned char key, int x, int y){
    switch(key) {
        case 'a':
            player_move(LEFT);
            break;
        case 'w':
            player_move(FORWARD);
            break;
        case 's':
            player_move(BACK);
            break;
        case 'd':
            player_move(RIGHT);
            break;
        case ' ':
            player_move(UP);
            break;
        case 'z':
            player_move(DOWN);
            break;
        case 'b':
            view_toggle_bcull();
            glutPostRedisplay();
            break;
        case 'q':
            controller_model->collapse_some_edges();
    }
}

void controller_keyboardUpFunc(unsigned char key, int x, int y){
    switch(key) {
        case 'a':
            player_stop(LEFT);
            break;
        case 'w':
            player_stop(FORWARD);
            break;
        case 's':
            player_stop(BACK);
            break;
        case 'd':
            player_stop(RIGHT);
            break;
        case ' ':
            player_stop(UP);
            break;
        case 'z':
            player_stop(DOWN);
            break;
    }
}

void controller_run() {
    glutMainLoop();
}
