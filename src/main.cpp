#include <iostream>
#include <fstream>
#include <cmath>
#include <GL/glut.h>

#include "view.hpp"
#include "model.hpp"
#include "controller.hpp"

using namespace std;

int main(int argc, char** argv)
{
    controller_init(argc, argv);
    controller_run();
}
