#ifndef MODEL_HPP
#define MODEL_HPP
#include <string>
#include <vector>

#include "geom.hpp"

struct HE_vert;
struct HE_face;

struct HE_edge {
    unsigned int index;
    HE_vert *vert;
    HE_face *face;
    HE_edge *next;
    HE_edge *prev;
    HE_edge *pair;
    bool deleted;
    double cost; // Memoized value
};

struct HE_vert {
    unsigned int index;
    Point point;
    HE_edge *edge;
    bool deleted;
};

struct HE_face {
    unsigned int index;
    HE_edge *edge;
    bool deleted;
};

class Model {
    public:
        Model();
        Model(const Model&);
        Model(const std::string);

        ~Model();

        void operator=(const Model&);

        void display();
        void collapse_some_edge();
        void error_versus(Model&);

    private:
        HE_vert *verts;
        HE_face *faces;
        HE_edge *edges;

        unsigned int num_verts, num_faces, num_edges;

        Vector normal(HE_face *face);
        double edge_dec_cost(HE_edge *edge);
        void collapse_edge(HE_edge *edge);
};
#endif
