#include <algorithm>
#include <set>
#include <iostream>
#include <fstream>
#include <GL/gl.h>
#include <GL/glut.h>
#include <float.h>

#include "model.hpp"

Model::Model() {
    num_verts = num_faces = num_edges = -1;
    verts = NULL;
    faces = NULL;
    edges = NULL;
}

Model::Model(const Model& other) {
    num_verts = other.num_verts;
    num_faces = other.num_faces;
    num_edges = other.num_edges;

    verts = new HE_vert[num_verts];
    faces = new HE_face[num_faces];
    edges = new HE_edge[num_edges];

    std::copy(other.verts, other.verts + num_verts, verts);
    std::copy(other.faces, other.faces + num_faces, faces);
    std::copy(other.edges, other.edges + num_edges, edges);
}

Model::Model(const std::string filename) {
    std::ifstream fp_in;
    float vx,vy,vz;

    fp_in.open(filename.c_str());
    if(fp_in.fail()) {
        std::cout << "Error opening mesh file" << std::endl;
        exit(1);
    }

    fp_in.ignore(INT_MAX, '\n');                //ignore first line
    fp_in >> num_verts >> num_faces >> num_edges;

    verts = new HE_vert[num_verts];

    for (unsigned int i = 0; i < num_verts; i++) {
        fp_in >> vx >> vy >> vz;
        verts[i].point = Point(vx,vy,vz);
        verts[i].edge = NULL;
        verts[i].deleted = false;
        verts[i].index = i;
    }

    num_edges = num_faces * 3;
    
    faces = new HE_face[num_faces];
    // Need extra space for boundary edges.  Requirements unknown but at most the same as the number of edges.
    edges = new HE_edge[2*num_edges];

    unsigned int num, k[3], l;
    for (unsigned int i = 0; i < num_faces; i++) {
        fp_in >> num >> k[0] >> k[1] >> k[2];

        if (num != 3) {
            std::cout << "ERROR: Polygon with index " << i  << " is not a triangle." << std::endl;  //Not a triangle!!
            exit(1);
        }

        for (unsigned int j = 0; j < 3; j++) {
            edges[3*i+j].vert = &verts[k[j]];
            verts[k[j]].edge = &edges[3*i+j];
            edges[3*i+j].next = &edges[3*i+(j+1)%3];
            edges[3*i+j].prev = &edges[3*i+(j+2)%3];
            edges[3*i+j].face = &faces[i];
            edges[3*i+j].pair = NULL;
            edges[3*i+j].deleted = false;
            edges[3*i+j].index = 3*i+j;
            edges[3*i+j].cost = -1.0;
        }

        faces[i].edge = &edges[3*i];
        faces[i].deleted = false;
        faces[i].index = i;

        l = 3*(i+1);
    }
    
    fp_in.close();

    std::cout << " File successfully read." << std::endl;

    for (unsigned int i = 0; i < num_edges; i++) {
        if (edges[i].pair == NULL) {
            for (unsigned int j = i+1; j < num_edges; j++) {
                if (edges[j].vert == edges[i].prev->vert && edges[j].prev->vert == edges[i].vert) {
                    edges[i].pair = &edges[j];
                    edges[j].pair = &edges[i];
                    break;
                }
            }
            // No edge found so create a boundary edge.
            if (edges[i].pair == NULL) {
                edges[i].pair = &edges[l];

                edges[l].vert = edges[i].prev->vert;

                HE_edge *e1 = &edges[i];
                do {
                    e1 = e1->prev->pair;
                } while(e1 != NULL && e1->face != NULL);

                if (e1 != NULL) {
                    edges[l].next = e1;
                    e1->prev = &edges[l];
                }

                HE_edge *e2 = &edges[i];
                do {
                    e2 = e2->next->pair;
                } while(e2 != NULL && e2->face != NULL);

                if (e2 != NULL) {
                    edges[l].prev = e2;
                    e2->next = &edges[l];
                }

                edges[l].face = NULL;
                edges[l].pair = &edges[i];
                edges[l].deleted = false;
                edges[l].index = l;

                l++;
            }
        }
    }

    std::cout << "Found all pairs." << std::endl;

    for (unsigned int i = 0; i < num_edges * 2; i++) {
        if (edges[i].pair != NULL && edges[i].prev == NULL) {
            std::cout << "Found edge [" << i << "] without a prev." << std::endl;
        }
        if (edges[i].pair != NULL && edges[i].next == NULL) {
            std::cout << "Found edge [" << i << "] without a next." << std::endl;
        }
    }

    current_edges = num_edges;
}

Model::~Model() {
    delete[] verts;
    delete[] faces;
    delete[] edges;
}
void Model::operator=(const Model& other) {
    delete[] verts;
    delete[] faces;
    delete[] edges;

    num_verts = other.num_verts;
    num_faces = other.num_faces;
    num_edges = other.num_edges;

    verts = new HE_vert[num_verts];
    faces = new HE_face[num_faces];
    edges = new HE_edge[num_edges];

    std::copy(other.verts, other.verts + num_verts, verts);
    std::copy(other.faces, other.faces + num_faces, faces);
    std::copy(other.edges, other.edges + num_edges, edges);
}

void Model::display() {
    HE_face face;
    HE_edge *edge;
    Point point;

    glBegin(GL_TRIANGLES);
    for (unsigned int i = 0; i < num_faces; i++) {
        face = faces[i];

        if (! face.deleted) {
            edge = face.edge;
            if (edge->deleted)
                std::cout << "Deleted edge [" << edge->index << "] printed as part of face [" << face.index << "]." << std::endl;
            if (edge->vert->deleted)
                std::cout << "Deleted vertex [" << edge->vert->index << "] printed from edge [" << edge->index << "] as part of face [" << face.index << "]." << std::endl;
            point = edge->vert->point;
            glVertex3d(point.x, point.y, point.z);

            edge = edge->next;
            if (edge->deleted)
                std::cout << "Deleted edge [" << edge->index << "] printed as part of face [" << face.index << "]." << std::endl;
            if (edge->vert->deleted)
                std::cout << "Deleted vertex [" << edge->vert->index << "] printed from edge [" << edge->index << "] as part of face [" << face.index << "]." << std::endl;
            point = edge->vert->point;
            glVertex3d(point.x, point.y, point.z);

            edge = edge->next;
            if (edge->deleted)
                std::cout << "Deleted edge [" << edge->index << "] printed as part of face [" << face.index << "]." << std::endl;
            if (edge->vert->deleted)
                std::cout << "Deleted vertex [" << edge->vert->index << "] printed from edge [" << edge->index << "] as part of face [" << face.index << "]." << std::endl;
            point = edge->vert->point;
            glVertex3d(point.x, point.y, point.z);
        }
    }
    glEnd();
}

std::set<HE_vert*> intersection(std::set<HE_vert*> set1, std::set<HE_vert*> set2) {
    std::set<HE_vert*> result = std::set<HE_vert*>();
    for (std::set<HE_vert*>::iterator it = set1.begin(); it != set1.end(); it++) {
        if (set2.find(*it) != set2.end()) {
            result.insert(*it);
        }
    }
    return result;
}

std::set<HE_vert*> one_ring(HE_edge *edge) {
    std::set<HE_vert*> result = std::set<HE_vert*>();
    HE_edge *e0 = edge,
            *e = e0;

    std::cout << "Starting one_ring at [" << edge->index << "]" << std::endl;

    do {
        result.insert(e->pair->vert);
        e = e->pair->prev;
        std::cout << "Moving around on_ring to [" << e->index << "]" << std::endl;
        if (e->deleted) std::cout << "Deleted edge [" << e->index << "] in one-ring." << std::endl;
    } while(e != e0);

    return result;
}

std::set<HE_vert*> two_ring(HE_edge *edge) {
    std::set<HE_vert*> result = std::set<HE_vert*>();

    std::set<HE_vert*> first_ring = one_ring(edge);

    for (std::set<HE_vert*>::iterator it = first_ring.begin(); it != first_ring.end(); it++) {
        // std::cout << "vert is deleted? [" << (*it)->deleted << "]." << std::endl;
        // std::cout << "edge is deleted? [" << (*it)->edge->deleted << "]." << std::endl;
        std::set<HE_vert*> temp_ring = one_ring((*it)->edge);
        for (std::set<HE_vert*>::iterator it = temp_ring.begin(); it != temp_ring.end(); it++) {
            if ((*it)->deleted) std::cout << "Deleted vert [" << (*it)->index << "] in two-ring." << std::endl;
            result.insert(*it);
        }
    }

    return result;
}

void Model::collapse_some_edge() {
    //std::cout << "Finding edge to collapse." << std::endl;

    double start_time = clock() / (double)CLOCKS_PER_SEC;

    HE_edge *best_edge = &edges[0];

    double current_score;
    for (unsigned int i = 1; i < num_edges; i++) {
        current_score = edge_dec_cost(&edges[i]);
        if (current_score < edge_dec_cost(best_edge)) {
            best_edge = &edges[i];
        }
    }

    double time = clock() / (double)CLOCKS_PER_SEC - start_time;

    if (edge_dec_cost(best_edge) < DBL_MAX) {
        //std::cout << "Found edge [" << best_edge->index << "] with score [" << edge_dec_cost(best_edge) << "] to collapse in [" << time << "] seconds." << std::endl;
        collapse_edge(best_edge);
        current_edges -= 6;
        glutPostRedisplay();
    } else {
        std::cout << "No edge left to collapse." << std::endl;
    }
}

static Model* model_to_collapse = NULL;

void collapse_next_edge(int num_to_go) {
    if (num_to_go == 0) {
        model_to_collapse = NULL;
    } else {
        std::cout << "[" << num_to_go << "] edges left to collapse." << std::endl;
        model_to_collapse->collapse_some_edge();
        glutTimerFunc(50, collapse_next_edge, num_to_go - 1);
    }
}

void Model::collapse_some_edges() {
    if (model_to_collapse == NULL) {
        model_to_collapse = this;
        glutTimerFunc(50, collapse_next_edge, current_edges/8);
    }
}

void Model::collapse_edge(HE_edge *edge) {
    HE_edge *e1 = edge,
            *e2 = e1->pair,
            *a  = e1->prev,
            *b1 = e1->next,
            *b2 = b1->pair,
            *c  = e2->next,
            *d1 = e2->prev,
            *d2 = d1->pair;

    HE_vert *p = e2->vert,
            *q = e1->vert;

    std::cout << "e1 [" << e1->index << "]" << std::endl
              << "e2 [" << e2->index << "]" << std::endl
              << "a  [" << a->index << "]" << std::endl
              << "b1 [" << b1->index << "]" << std::endl
              << "b2 [" << b2->index << "]" << std::endl
              << "c  [" << c->index << "]" << std::endl
              << "d1 [" << d1->index << "]" << std::endl
              << "d2 [" << d2->index << "]" << std::endl
              << "p  [" << p->index << "]" << std::endl
              << "q  [" << q->index << "]" << std::endl
              << "f  [" << a->pair->index << "]" << std::endl
              << "g  [" << a->pair->next->index << "]" << std::endl
              << "h  [" << a->pair->prev->index << "]" << std::endl
              << "i  [" << c->pair->index << "]" << std::endl
              << "j  [" << c->pair->next->index << "]" << std::endl
              << "j->pair  [" << c->pair->next->pair->index << "]" << std::endl
              << "k  [" << c->pair->prev->index << "]" << std::endl
              << "x  [" << d2->prev->index << "]" << std::endl;


    a->prev = b2->prev;
    a->next = b2->next;
    if (b2->next == d2) a->next = c;
    a->face = b2->face;
    c->prev = d2->prev;
    if (d2->prev == b2) c->prev = a;
    c->next = d2->next;
    c->face = d2->face;

    if (p->edge == e2) {
        p->edge = a;
    }

    edge = d1;
    do {
        edge = edge->pair->prev;
        edge->vert = p;
    } while (edge != b2);

    b2->prev->next = a; b2->next->prev = a; b2->face->edge = a;
    d2->prev->next = c; d2->next->prev = c; d2->face->edge = c;

    b1->vert->edge = a->pair;
    d2->vert->edge = c;

    p->point = p->point + 0.5 * (q->point - p->point);

    e1->deleted = true;
    e2->deleted = true;
    b1->deleted = true;
    b2->deleted = true;
    d1->deleted = true;
    d2->deleted = true;
    e1->face->deleted = true;
    e2->face->deleted = true;
    q->deleted = true;


    // Reset memoized cost on the two_ring of the new point.
    std::set<HE_vert*> ring = two_ring(a);
    for (std::set<HE_vert*>::iterator it = ring.begin(); it != ring.end(); it++) {
        HE_edge *e0 = (*it)->edge,
                *e1 = e0;
        do {
            e1->cost = -1.0;
            e1 = e1->pair->prev;
        } while (e1 != e0);
    }
}


Vector Model::normal(HE_face *face) {
    HE_edge *edge = face->edge;

    Point point1 = edge->vert->point,
          point2 = edge->next->vert->point,
          point3 = edge->prev->vert->point;

    return (point2 - point1).cross(point3 - point1).unit();
}

double Model::edge_dec_cost(HE_edge *edge) {
    if (edge->deleted) {
        return DBL_MAX;
    }

    // Return memoized cost if it is set.
    if (edge->cost > 0.0) {
        return edge->cost;
    }

    // Boundary edge
    if (edge->pair->face == NULL) {
        edge->cost = DBL_MAX;
        return edge->cost;
    }

    // Edge with any boundary vertex
    HE_edge *e0 = edge, *e = e0;
    do {
        e = e->pair->prev;
        if (e->face == NULL || e->pair->face == NULL) {
            edge->cost = DBL_MAX;
            return edge->cost;
        }
    } while (e != e0);

    e0 = edge->pair, e = e0;

    do {
        e = e->pair->prev;
        if (e->face == NULL || e->pair->face == NULL) {
            edge->cost = DBL_MAX;
            return edge->cost;
        }
    } while (e != e0);

    // Edge with more than two vertices in one-ring neighbourhood of end points
    std::cout << "Intersection contains [" << intersection(one_ring(edge), one_ring(edge->pair)).size() << "] vertices." << std::endl;
    if (intersection(one_ring(edge), one_ring(edge->pair)).size() > 2) {
        edge->cost = DBL_MAX;
        return edge->cost;
    }
    
    HE_edge *pair = edge->pair;

    Vector m1 = normal(edge->face),
           m2 = normal(pair->face);

    double angle_error = (1.0 - m1.dot(m2)) / 2.0;

    double length = (edge->vert->point - pair->vert->point).length();

    edge->cost = 1.0 * angle_error + 1.0 * length;
    return edge->cost;
}
