#ifndef MESH_H
#define MESH_H

#include <vector>
#include <list>
#include <vec3d.h>
#include <Cube.h>
#include <CubeGrid.h>
#include <Vertex.h>
#include <Triangle.h>
#include <set>

using namespace std;

// Generates and stores the mesh from a cube grid
// with its vertices and triangles

class Mesh
{
    public:
        float dimX, dimY, dimZ;
        vector<Vertex> vertices;
        vector<Triangle> triangles;
        vector<list<unsigned int>> sharedTriangles;

        Mesh();
        void generateMesh(CubeGrid &cubeGrid, unsigned int minTriangleCount);
        void clear();
        // get the vertex and indices arrays to load in the buffers
        float* getVertexArray();
        unsigned int* getTriangleArray();
        virtual ~Mesh();

    protected:

    private:
        // create the triangles of a cube according to its configuration
        void triangulateCube(Cube cube);
        void calculateNormals();
        // create an adjacency list of the shared triangles by each vertex
        void assignSharedTriangles();
        // remove triangles that are part of small unwanted subshapes
        void ignoreSmallRegions(unsigned int minTriangleCount);
        // utility method for the ignoreSmallRegions method : apply floodfill to get
        // triangles belonging to a same region starting from a given vertex
        void getRegionTriangles(int startVertex, bool *visited, set<int>& regionTriangles);
};

#endif // MESH_H
