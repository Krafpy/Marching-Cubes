#ifndef CUBE_H
#define CUBE_H

#include <vec3d.h>
#include <ControlNode.h>
#include <vector>
#include <utility>
#include <Vertex.h>

using namespace std;


// cube class to generate and store a cube
// with its edges' nodes

class Cube
{
    public:
        ControlNode **controlNodes;
        int *edgeNodes;
        int configuration;
        int bordering;

        Cube();
        Cube(ControlNode **_controlNodes, int _bordering);

        // create the necessary edge vertices of the cube
        void createVertices(vector<Vertex>& vertices, float surfaceLevel);

        virtual ~Cube();

    protected:

    private:
        vec3d interpolate(int a, int b, float t);
};

#endif // CUBE_H
