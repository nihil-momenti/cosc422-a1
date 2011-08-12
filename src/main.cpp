#include <iostream>
#include <fstream>
#include <cmath>
#include <GL/glut.h>

#include "view.hpp"
#include "model.hpp"
#include "controller.hpp"

int main(int argc, char** argv)
{
    controller_init(argc, argv);
    controller_run();
}
