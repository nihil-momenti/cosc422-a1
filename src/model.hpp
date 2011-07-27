#include <string>
#include <vector>

struct HE_edge {
  int vert; 
  int face; 
  int next; 
  int pair;
};

struct HE_vert {
  float x; 
  float y;
  float z;
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
