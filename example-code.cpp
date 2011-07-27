/* 
   Program to load and display files in OFF format

   Uses half-edge data structure instead of vertex/face lists for rendering the model

   Keys:  LEFT, RIGHT: rotate about y-axis
          UP, DOWN:  zoom in/out
          PAGEUP, PAGEDOWN: rotate about x-axis

          'b': Enable/disable backface culling

    Input model file:  camel.off


    R.Mukundan(2008)
*/

#include <iostream>
#include <fstream>
#include <cmath>
#include <GL/glut.h>

using namespace std;
float xrot=0.0, yrot=0.0, fov=30.0;
int nv=0, np=0,bcull=0;

struct HE_vert;
struct HE_face;

struct HE_edge {
  HE_vert* vert; 
  HE_face* face; 
  HE_edge* next; 
  HE_edge* pair;
};

struct HE_vert {
  float x; 
  float y;
  float z;
  HE_edge* edge;
};

struct HE_face {
  HE_edge* edge;
};

HE_vert *vertArray;
HE_face *faceArray;
HE_edge *edgeArray;


//============================================================

void init()  
{
	ifstream fp_in;
	int ne;
	float vx,vy,vz;

	fp_in.open("camel.off", ios::in);
	if(!fp_in.is_open())
	{
		cout << "Error opening mesh file" << endl;
		exit(1);
	}

	fp_in.ignore(INT_MAX, '\n');				//ignore first line
	fp_in >> nv >> np >> ne;					// Read No. of vertices, polygons, edges

	vertArray = new HE_vert[nv];

	HE_vert *vert;
	for(int i=0; i<nv; i++){                         //Construct vertex array 
		fp_in >> vx >> vy >> vz;
		vert = vertArray+i;
		vert->x = vx;
		vert->y = vy;
		vert->z = vz;
		vert->edge = NULL;
	}

	ne = np*3;				//Each triangle has 3 distinct edges
	
	faceArray = new HE_face[np];
	edgeArray = new HE_edge[ne];

	HE_face *face;
	HE_edge *edge1, *edge2, *edge3;
	int j=0;  //edge index
	int num, k1, k2, k3;
	for(int i=0; i<np; i++){                         //Construct face, edge arrays
		fp_in >> num >> k1 >> k2 >> k3;
		if(num!=3) {
			cout << "ERROR: Polygon with index " << i  << " is not a triangle." << endl;  //Not a triangle!!
			exit(1);
		}
		face = faceArray+i;
		edge1 = edgeArray+j;
		edge2 = edge1+1;
		edge3 = edge2+1;
		edge1->vert=vertArray+k2;
		edge1->next=edge2;
		edge1->face=face;
		edge2->vert=vertArray+k3;
		edge2->next=edge3;
		edge2->face=face;
		edge3->vert=vertArray+k1;
		edge3->next=edge1;
		edge3->face=face;
		edge1->pair=NULL;   //IMPORTANT: THESE MUST BE UPDATED BEFORE PROCESSING THE MESH!!
		edge2->pair=NULL;
		edge3->pair=NULL;
		face->edge=edge1;
		j+=3;
	}
	fp_in.close();

	cout << " File successfully read." << endl;


	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); 	
	glClearDepth(1.0f);  	
	glEnable(GL_DEPTH_TEST);  
	glEnable(GL_NORMALIZE);
}
//============================================================

void myDisplay()  
{
	int i;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov,1.,2.,100.);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();  	
	gluLookAt(0.,0.,3.,0.,0.,0.,0.,1.,0.);
    if(bcull){
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
	} else 
		glDisable(GL_CULL_FACE);


	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor3f(1.,0.,0.);


	glRotatef(yrot,0.0f,1.0f,0.0f);  
	glRotatef(xrot,1.0f,0.0f,0.0f);

	HE_face *face;
	HE_edge *edge;
	HE_vert *vert;

	glBegin(GL_TRIANGLES);
	for(i=0; i<np; i++){
		   face = faceArray+i;
		   edge = face->edge;
		   vert = edge->vert;
		   glVertex3d(vert->x, vert->y, vert->z);
		   edge = edge->next;
		   vert = edge->vert;
		   glVertex3d(vert->x, vert->y, vert->z);
		   edge = edge->next;
		   vert = edge->vert;
		   glVertex3d(vert->x, vert->y, vert->z);

	}
	glEnd();

	glFlush();
	glutSwapBuffers();
}
//------------------------------------------------------------------- 
void myKeys(unsigned char key, int x, int y){

	if(key=='b') bcull=(bcull+1)%2;

	glutPostRedisplay();
}
//----------------------------------------------------------------

void splFun(int k, int x, int y){
	switch(k){
	case GLUT_KEY_UP: fov-=2.0; break;
	case GLUT_KEY_DOWN: fov+=2.0; break;
	case GLUT_KEY_LEFT: yrot-=5.0; break;
	case GLUT_KEY_RIGHT: yrot+=5.0; break;
	case GLUT_KEY_PAGE_UP: xrot-=5.0; break;
	case GLUT_KEY_PAGE_DOWN: xrot+=5.0; break;

	}
	if(fov<2.0) fov=2.0; 
	glutPostRedisplay();
}

//----------------------------------------------------------------


int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB|GLUT_DEPTH);
   glutInitWindowSize (800, 800); 
   glutInitWindowPosition (10, 10);
   glutCreateWindow (argv[0]);
   init ();

   glutDisplayFunc(myDisplay); 
   glutSpecialFunc(splFun);
   glutKeyboardFunc(myKeys);
   glutMainLoop();
   return 0;
}
