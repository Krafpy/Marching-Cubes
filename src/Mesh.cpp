#include "Mesh.h"

#include <Table.h>
#include <Cube.h>
#include <queue>
#include <set>
#include <algorithm>

using namespace std;


Mesh::Mesh()
{

}

void Mesh::generateMesh(CubeGrid& cubeGrid, unsigned int minTriangleCount)
{
    dimX = cubeGrid.width * cubeGrid.cubeSize;
    dimY = cubeGrid.height * cubeGrid.cubeSize;
    dimZ = cubeGrid.depth * cubeGrid.cubeSize;

    for(int i = 0; i < cubeGrid.width; i++){
        for(int j = 0; j < cubeGrid.height; j++){
            for(int k = 0; k < cubeGrid.depth; k++){
                triangulateCube(cubeGrid.cubes[i][j][k]);
            }
        }
    }

    assignSharedTriangles();

    if(minTriangleCount > 0){
        ignoreSmallRegions(minTriangleCount);
        // reassign sharing because the triangles vector may have changed
        assignSharedTriangles();
    }

    calculateNormals();
}

void Mesh::clear()
{
    vertices.clear();
    triangles.clear();
    sharedTriangles.clear();
}

void Mesh::calculateNormals()
{
    // the normal of each vertex is calculated as the average of each normals
    // of the triangles sharing this vertex

    for(auto it = triangles.begin(); it != triangles.end(); ++it){
        Triangle& triangle = *it;

        vec3d& v1 = vertices[triangle.a].pos;
        vec3d& v2 = vertices[triangle.b].pos;
        vec3d& v3 = vertices[triangle.c].pos;

        triangle.normal = vec3d::cross(v2-v1, v3-v1);
        triangle.normal.normalize();
    }

    for(unsigned int i = 0; i < vertices.size(); i++){
        Vertex& vertex = vertices[i];

        for(auto it = sharedTriangles[i].begin(); it != sharedTriangles[i].end(); ++it){
            vertex.normal += triangles[*it].normal;
        }

        vertex.normal.normalize();
    }
}

void Mesh::assignSharedTriangles()
{
    sharedTriangles.clear();
    sharedTriangles.resize(vertices.size());

    for(unsigned int i = 0; i < triangles.size(); i++){
        sharedTriangles[triangles[i].a].push_back(i);
        sharedTriangles[triangles[i].b].push_back(i);
        sharedTriangles[triangles[i].c].push_back(i);
    }
}

void Mesh::triangulateCube(Cube cube)
{
    const int *triConfig = triTable[cube.configuration];

    int a, b, c;

    for(int i = 0; triConfig[i] != -1; i += 3){
        a = cube.edgeNodes[triConfig[i]];
        b = cube.edgeNodes[triConfig[i+1]];
        c = cube.edgeNodes[triConfig[i+2]];
        triangles.push_back(Triangle(a, b, c));
    }
}

void Mesh::ignoreSmallRegions(unsigned int minTriangleCount)
{
    // Apply floodfill on each non already visited vertices to get a
    // vector of triangles (referred as their index) to delete.
    // This step completes the per node floodfill as it clears features
    // not detected in the previous step.

    bool *visited = new bool[vertices.size()];
    for(unsigned int i = 0; i < vertices.size(); i++){
        visited[i] = false;
    }

    // list of all triangles that should be deleted

    vector<int> trianglesToDelete;

    for(unsigned int i = 0; i < vertices.size(); i++){
        if(!visited[i]){
            visited[i] = true;

            set<int> regionTriangles;

            getRegionTriangles(i, visited, regionTriangles);

            if(regionTriangles.size() < minTriangleCount){
                for(auto tri = regionTriangles.begin(); tri != regionTriangles.end(); ++tri){
                    trianglesToDelete.push_back(*tri);
                }
            }
        }
    }

    delete[] visited;

    // delete the triangles from the triangles vector

    sort(trianglesToDelete.begin(), trianglesToDelete.end());

    for(auto tri = trianglesToDelete.end() - 1; tri != trianglesToDelete.begin() - 1; tri--){
        iter_swap(triangles.begin() + (*tri), triangles.end() - 1);
        triangles.pop_back();
    }
}

void Mesh::getRegionTriangles(int startVertex, bool *visited, set<int>& regionTriangles)
{
    // floodfill algorithm visiting all vertices of a connected shape

    queue<int> vertQ;

    int vertex;
    int va, vb, vc;

    vertQ.push(startVertex);

    while(!vertQ.empty()){

        vertex = vertQ.front();
        vertQ.pop();

        for(auto tri = sharedTriangles[vertex].begin(); tri != sharedTriangles[vertex].end(); ++tri){
            regionTriangles.insert(*tri);

            va = triangles[*tri].a;
            vb = triangles[*tri].b;
            vc = triangles[*tri].c;

            if(!visited[va]){
                visited[va] = true;
                vertQ.push(va);
            }
            if(!visited[vb]){
                visited[vb] = true;
                vertQ.push(vb);
            }
            if(!visited[vc]){
                visited[vc] = true;
                vertQ.push(vc);
            }
        }
    }
}

float* Mesh::getVertexArray()
{
    // generate the array of vertex data (position and normal)
    // to load in the VBO

    float *vertexData = new float[vertices.size()*2*3];

    int index;

    for(unsigned int i = 0; i < vertices.size(); i++){

        index = i * 3;

        vertexData[index] = vertices[i].pos.x;
        vertexData[index+1] = vertices[i].pos.y;
        vertexData[index+2] = vertices[i].pos.z;
    }

    for(unsigned int i = 0; i < vertices.size(); i++){

        index = (vertices.size() + i) * 3;

        vertexData[index] = vertices[i].normal.x;
        vertexData[index+1] = vertices[i].normal.y;
        vertexData[index+2] = vertices[i].normal.z;
    }

    return vertexData;
}

unsigned int* Mesh::getTriangleArray()
{
    // generate the indices array to load in the IBO
    // to draw the corresponding triangles

    unsigned int *trianglesData = new unsigned int[triangles.size()*3];

    int index;

    for(unsigned int i = 0; i < triangles.size(); i++){

        index = i*3;

        trianglesData[index] = triangles[i].a;
        trianglesData[index+1] = triangles[i].b;
        trianglesData[index+2] = triangles[i].c;
    }

    return trianglesData;
}

Mesh::~Mesh()
{

}
