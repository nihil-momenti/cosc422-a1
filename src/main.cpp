#include <iostream>
#include <fstream>
#include <cmath>
#include <GL/glut.h>

#include "view.hpp"
#include "model.hpp"
#include "controller.hpp"

int main(int argc, char** argv)
{
    Model model = Model("models/camel.off");
    controller_init(&model, argc, argv);
    controller_run();
}
