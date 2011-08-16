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

    for (int i = 0; i < num_verts; i++) {
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

    int num, k1, k2, k3, l;
    for (int i = 0, j = 0; i < num_faces; i++, j+=3) {
        fp_in >> num >> k1 >> k2 >> k3;

        if (num != 3) {
            std::cout << "ERROR: Polygon with index " << i  << " is not a triangle." << std::endl;  //Not a triangle!!
            exit(1);
        }

        edges[j].vert = &verts[k1];
        verts[k1].edge = &edges[j];
        edges[j].next = &edges[j+1];
        edges[j].prev = &edges[j+2];
        edges[j].face = &faces[i];
        edges[j].pair = NULL;
        edges[j].deleted = false;
        edges[j].index = j;

        edges[j+1].vert = &verts[k2];
        verts[k2].edge = &edges[j+1];
        edges[j+1].next = &edges[j+2];
        edges[j+1].prev = &edges[j];
        edges[j+1].face = &faces[i];
        edges[j+1].pair = NULL;
        edges[j+1].deleted = false;
        edges[j+1].index = j+1;

        edges[j+2].vert = &verts[k3];
        verts[k3].edge = &edges[j+2];
        edges[j+2].next = &edges[j];
        edges[j+2].prev = &edges[j+1];
        edges[j+2].face = &faces[i];
        edges[j+2].pair = NULL;
        edges[j+2].deleted = false;
        edges[j+2].index = j+2;

        faces[i].edge = &edges[j];
        faces[i].deleted = false;
        faces[i].index = i;

        l = j+3;
    }
    
    fp_in.close();

    std::cout << " File successfully read." << std::endl;

    for (int i = 0; i < num_edges; i++) {
        if (edges[i].pair == NULL) {
            for (int j = i+1; j < num_edges; j++) {
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

    for (int i = 0; i < num_edges * 2; i++) {
        if (edges[i].pair != NULL && edges[i].prev == NULL) {
            std::cout << "Found edge [" << i << "] without a prev." << std::endl;
        }
        if (edges[i].pair != NULL && edges[i].next == NULL) {
            std::cout << "Found edge [" << i << "] without a next." << std::endl;
        }
    }
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
    for (int i = 0; i < num_faces; i++) {
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

void Model::collapse_some_edge() {
    std::cout << "Finding edge to decimate." << std::endl;

    double start_time = clock() / (double)CLOCKS_PER_SEC;

    HE_edge *best_edge = &edges[0];

    double current_score;
    for (int i = 1; i < num_edges; i++) {
        current_score = edge_dec_cost(&edges[i]);
        if (current_score < edge_dec_cost(best_edge)) {
            best_edge = &edges[i];
        }
    }

    double time = clock() / (double)CLOCKS_PER_SEC - start_time;

    if (edge_dec_cost(best_edge) < DBL_MAX) {
        std::cout << "Found edge [" << best_edge->index << "] with score [" << edge_dec_cost(best_edge) << "] to decimate in [" << time << "] seconds." << std::endl;
        collapse_edge(best_edge);
        glutPostRedisplay();
    } else {
        std::cout << "No edge left to decimate." << std::endl;
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

    a->prev = b2->prev;
    a->next = b2->next;
    a->face = b2->face;
    c->prev = d2->prev;
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

    b2->prev->next = a;
    b2->next->prev = a;
    b2->face->edge = a;
    d2->prev->next = c;
    d2->next->prev = c;
    d2->face->edge = c;

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
}


Vector Model::normal(HE_face *face) {
    HE_edge *edge = face->edge;

    Point point1 = edge->vert->point,
          point2 = edge->next->vert->point,
          point3 = edge->prev->vert->point;

    return (point2 - point1).cross(point3 - point1).unit();
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

    do {
        result.insert(e->pair->vert);
        e = e->pair->prev;
    } while(e != e0);

    return result;
}

double Model::edge_dec_cost(HE_edge *edge) {
    if (edge->deleted) {
        return DBL_MAX;
    }

    // Boundary edge
    if (edge->pair->face == NULL) {
        return DBL_MAX;
    }

    // Edge with any boundary vertex
    HE_edge *e0 = edge, *e = e0;
    do {
        e = e->pair->prev;
        if (e->face == NULL || e->pair->face == NULL) {
            return DBL_MAX;
        }
    } while (e != e0);

    e0 = edge->pair, e = e0;

    do {
        e = e->pair->prev;
        if (e->face == NULL || e->pair->face == NULL) {
            return DBL_MAX;
        }
    } while (e != e0);

    // Edge with more than two vertices in one-ring neighbourhood of end points
    if (intersection(one_ring(edge), one_ring(edge->pair)).size() > 2) {
        return DBL_MAX;
    }
    
    HE_edge *pair = edge->pair;

    Vector m1 = normal(edge->face),
           m2 = normal(pair->face);

    double angle_error = (1.0 - m1.dot(m2)) / 2.0;

    double length = (edge->vert->point - pair->vert->point).length();

    return 1.0 * angle_error + 1.0 * length;
}
