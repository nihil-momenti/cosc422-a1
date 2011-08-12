#ifndef VIEW_HPP
#define VIEW_HPP

#include "model.hpp"

void view_init(Model *model, int argc, char *argv[]);
void view_reshape(int new_width, int new_height);
void view_display();

int get_view_height();
int get_view_width();

void view_toggle_bcull();

#endif
