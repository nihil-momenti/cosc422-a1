#include "player.hpp"

#include <algorithm>
#include <set>

#include <GL/glut.h>
#include <math.h>
#include <time.h>
#include <iostream>

#include "geom.hpp"


#define PI 3.1415926535897932384626433832795


static Point player_position,
             player_lookat;
static Vector player_viewup;
static double player_sensitivity,
              player_speed;
static std::set<Direction> player_moving;
static bool player_in_motion;
static time_t player_update_time;

static void player_update(int value=0);
static void player_forward(double);
static void player_right(double);
static void player_up(double);

void player_init() {
    player_position = Point(0,0,3);
    player_lookat = Point(0,0,2);
    player_viewup = Vector(0,1,0);
    player_sensitivity = 1;
    player_speed = 0.02;
    player_in_motion = false;
}

void player_move(Direction direction) {
    if (player_moving.find(direction) != player_moving.end()) {
        return;
    }

    player_moving.insert(direction);

    if (! player_in_motion) {
        player_in_motion = true;
        player_update_time = (clock()/(double)CLOCKS_PER_SEC);
        player_update();
    }
}

void player_stop(Direction direction) {
    player_moving.erase(direction);
}

void player_update(int value) {
    if (player_moving.empty()) {
        player_in_motion = false;
        return;
    }

    for (std::set<Direction>::iterator it = player_moving.begin(); it != player_moving.end(); it++) {
        switch (*it) {
            case FORWARD:
                player_forward(player_speed * ((clock()/(double)CLOCKS_PER_SEC) - player_update_time));
                break;
            case BACK:
                player_forward(-player_speed * ((clock()/(double)CLOCKS_PER_SEC) - player_update_time));
                break;
            case RIGHT:
                player_right(player_speed * ((clock()/(double)CLOCKS_PER_SEC) - player_update_time));
                break;
            case LEFT:
                player_right(-player_speed * ((clock()/(double)CLOCKS_PER_SEC) - player_update_time));
                break;
            case UP:
                player_up(player_speed * ((clock()/(double)CLOCKS_PER_SEC) - player_update_time));
                break;
            case DOWN:
                player_up(-player_speed * ((clock()/(double)CLOCKS_PER_SEC) - player_update_time));
                break;
        }
    }

    player_update_time = (clock()/(double)CLOCKS_PER_SEC);
    glutTimerFunc(10, player_update, 0);
}

void player_look() {
    gluLookAt(
            player_position.x, player_position.y, player_position.z,
            player_lookat.x, player_lookat.y, player_lookat.z,
            player_viewup.dx, player_viewup.dy, player_viewup.dz
    );
}

static Vector player_facing() {
    return (player_lookat - player_position).unit();
}

static void player_forward(double amount) {
    Vector movement = amount * player_facing().unit();
    player_position = player_position + movement;
    player_lookat = player_lookat + movement;
    glutPostRedisplay();
}

static void player_right(double amount) {
    Vector movement = amount * player_facing().cross(player_viewup).unit();
    player_position = player_position + movement;
    player_lookat = player_lookat + movement;
    glutPostRedisplay();
}

static void player_up(double amount) {
    Vector movement = amount * player_viewup;
    player_position = player_position + movement;
    player_lookat = player_lookat + movement;
    glutPostRedisplay();
}

void player_turn(std::pair<double,double> rotation) {
    Vector original_look = player_lookat - player_position;

    double x = original_look.dx;
    double y = original_look.dy;
    double z = original_look.dz;

    double r = sqrt(x*x + y*y + z*z);

    double theta = std::max(0.01, std::min(PI - 0.01, acos(y / r) + player_sensitivity * rotation.second));
    double phi = atan2(z, x) + player_sensitivity * rotation.first;

    x = r * sin(theta) * cos(phi);
    y = r * cos(theta);
    z = r * sin(theta) * sin(phi);

    player_lookat = player_position + Vector(x, y, z).unit();
    glutPostRedisplay();
}
