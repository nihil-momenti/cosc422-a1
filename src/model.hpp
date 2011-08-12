#ifndef MODEL_HPP
#define MODEL_HPP
#include <string>
#include <vector>

#include "geom.hpp"

struct HE_edge {
  int vert; 
  int face; 
  int next; 
  int prev;
  int pair;
};

struct HE_vert {
    Point point;
    int edge;
};

struct HE_face {
  int edge;
};

class Model {
    public:
        Model();
        Model(std::string);
        void display();

    private:
        std::vector <HE_vert> verts;
        std::vector <HE_face> faces;
        std::vector <HE_edge> edges;
};
#endif
