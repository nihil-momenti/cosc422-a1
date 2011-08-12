#include <iostream>
#include <fstream>
#include <GL/gl.h>

#include "model.hpp"

using namespace std;

Model::Model() {
    verts = vector<HE_vert>();
    faces = vector<HE_face>();
    edges = vector<HE_edge>();
}

Model::Model(string filename) {
    ifstream fp_in;
    int number_vertices, number_faces, number_edges;
    float vx,vy,vz;

    fp_in.open(filename.c_str());
    if(fp_in.fail()) {
        cout << "Error opening mesh file" << endl;
        exit(1);
    }

    fp_in.ignore(INT_MAX, '\n');                //ignore first line
    fp_in >> number_vertices >> number_faces >> number_edges;

    verts = vector<HE_vert>(number_vertices);

    for (int i = 0; i < number_vertices; i++) {
        fp_in >> vx >> vy >> vz;
        verts[i].x = vx;
        verts[i].y = vy;
        verts[i].z = vz;
        verts[i].edge = -1;
    }

    number_edges = number_faces * 3;
    
    faces = vector<HE_face>(number_faces);
    edges = vector<HE_edge>(number_edges);

    int num, k1, k2, k3;
    for (int i = 0, j = 0; i < number_faces; i++, j+=3) {
        fp_in >> num >> k1 >> k2 >> k3;

        if (num != 3) {
            cout << "ERROR: Polygon with index " << i  << " is not a triangle." << endl;  //Not a triangle!!
            exit(1);
        }

        edges[j].vert = k2;
        edges[j].next = j+1;
        edges[j].prev = j+2;
        edges[j].face = i;
        edges[i].pair = -1;

        edges[j+1].vert = k3;
        edges[j+1].next = j+2;
        edges[j+1].prev = j;
        edges[j+1].face = i;
        edges[i+1].pair = -1;

        edges[j+2].vert = k1;
        edges[j+2].next = j;
        edges[j+2].prev = j+1;
        edges[j+2].face = i;
        edges[j+2].pair = -1;

        faces[i].edge = j;
    }
    
    fp_in.close();

    cout << " File successfully read." << endl;

    for (int i = 0; i < number_edges; i++) {
        if (edges[i].pair == -1) {
            for (int j = i+1; j < number_edges; j++) {
                if (edges[j].vert == edges[edges[i].prev].vert && edges[edges[j].prev].vert == edges[i].vert) {
                    edges[i].pair = j;
                    edges[j].pair = i;
                    break;
                }
            }
        }
    }
}

void Model::display() {
    HE_face face;
    HE_edge edge;
    HE_vert vert;

    glBegin(GL_TRIANGLES);
    for (std::vector<HE_face>::iterator it = faces.begin(); it != faces.end(); it++) {
        face = *it;

        edge = edges[face.edge];
        vert = verts[edge.vert];
        glVertex3d(vert.x, vert.y, vert.z);

        edge = edges[edge.next];
        vert = verts[edge.vert];
        glVertex3d(vert.x, vert.y, vert.z);

        edge = edges[edge.next];
        vert = verts[edge.vert];
        glVertex3d(vert.x, vert.y, vert.z);
    }
    glEnd();
}
