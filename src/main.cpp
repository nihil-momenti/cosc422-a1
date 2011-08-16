#include <iostream>
#include <fstream>
#include <cmath>
#include <GL/glut.h>

#include "view.hpp"
#include "model.hpp"
#include "controller.hpp"

int main(int argc, char** argv)
{
    std::string model_file;

    if (argc > 1) {
        model_file = std::string(argv[1]);
    } else {
        model_file = "models/camel.off";
    }

    Model model = Model(model_file);
    controller_init(&model, argc, argv);
    controller_run();
}
