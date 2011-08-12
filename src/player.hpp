#ifndef PLAYER_H
#define PLAYER_H

#include <algorithm>

typedef enum {
    FORWARD,
    LEFT,
    RIGHT,
    BACK,
    UP,
    DOWN
} Direction;

void player_init();
void player_move(Direction);
void player_stop(Direction);
void player_turn(std::pair<double,double>);
void player_look();

#endif
