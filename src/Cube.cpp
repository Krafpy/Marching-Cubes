#include "Cube.h"

#include <vec3d.h>
#include <ControlNode.h>
#include <vector>
#include <set>
#include <Table.h>
#include <Vertex.h>

using namespace std;


Cube::Cube()
{

}

Cube::Cube(ControlNode **_controlNodes, int _bordering) : controlNodes(_controlNodes), bordering(_bordering)
{
    // calculate the configuration of the current cube
    configuration = 0;
    for(int i = 0; i < 8; i++){
        if(controlNodes[i]->active){
            configuration |= 1 << i;
        }
    }

    edgeNodes = new int[12];
    /*for(int i = 0; i < 12; i++){
        edgeNodes[i] = -1;
    }*/
}

void Cube::createVertices(vector<Vertex>& vertices, float surfaceLevel)
{
    // which vertex to draw in addition to the 3 necessary ones
    // according to the position of the cube, i.e. vertices
    // that can't be determined by surrounding cubes
    static const int bordTable[8][9] = {
        {-1, -1,  -1, -1, -1, -1, -1, -1, -1 },    // 0
        {2, 11, -1, -1, -1, -1, -1, -1, -1 },      // 1
        {9, 1,  -1, -1, -1, -1, -1, -1, -1 },      // 2
        {2, 11, 9, 1, 10, -1, -1, -1, -1 },        // 3
        {4, 7,  -1, -1, -1,  -1, -1, -1, -1 },     // 4
        {2, 11, 4, 7, 6, -1, -1, -1, -1 },         // 5
        {4, 7,  1, 9, 5, -1, -1, -1, -1 },         // 6
        {2, 11, 4, 7, 6,  5, 10, 9, 1}             // 7
    };

    set<int> additionalEdges;

    for(int i = 0; i < 9 && bordTable[bordering][i] != -1; i++){
        additionalEdges.insert(bordTable[bordering][i]);
    }

    const int *triConfig = triTable[configuration];
    set<int> edges; // store all edges that have to be calculated
    int edge; // store a local edge index

    // only add edges that exist according to the triangulation table
    // and that are necessary

    for(int i = 0; triConfig[i] != -1; i++){
        edge = triConfig[i];
        if(edge == 0 || edge == 3 || edge == 8 || additionalEdges.find(edge) != additionalEdges.end())
            edges.insert(edge);
    }

    int a, b; // edge extremities
    int vertexIndex; // store the global index of an edge/vertex

    // calculate the vertices

    for(auto it = edges.begin(); it != edges.end(); ++it){
        edge = *it;

        // determine the correct extremities

        a = edge;

        if(edge <= 3){
            b = a == 3 ? 0 : a + 1;
        } else if(edge <= 7){
            b = a == 7 ? 4 : a + 1;
        } else {
            a -= 8;
            b = a + 4;
        }

        vertexIndex = vertices.size();

        edgeNodes[edge] = vertexIndex;

        vertices.push_back(Vertex(interpolate(a, b, surfaceLevel)));
    }
}

vec3d Cube::interpolate(int a, int b, float t)
{
    // returns the vertex at the corresponding edge
    // with interpolated coordinates

    vec3d& p1 = controlNodes[a]->pos;
    vec3d& p2 = controlNodes[b]->pos;

    float v1 = controlNodes[a]->value;
    float v2 = controlNodes[b]->value;

    return p1 + (t - v1) * (p2 - p1) / (v2 - v1);
}

Cube::~Cube()
{

}
