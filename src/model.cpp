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
        verts[i].edge = NULL;
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
        edges[j].face = i;
        edges[i].pair = NULL;

        edges[j+1].vert = k3;
        edges[j+1].next = j+2;
        edges[j+1].face = i;
        edges[i+1].pair = NULL;

        edges[j+2].vert = k1;
        edges[j+2].next = j;
        edges[j+2].face = i;
        edges[j+2].pair = NULL;

        faces[i].edge = j;
    }
    
    fp_in.close();

    cout << " File successfully read." << endl;
}

void Model::display() {
    HE_face face;
    HE_edge edge;
    HE_vert vert;

    glBegin(GL_TRIANGLES);
    for (size_t i = 0; i < faces.size(); i++) {
        face = faces[i];

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
