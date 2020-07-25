#include "CubeGrid.h"

#include <vec3d.h>
#include <Cube.h>
#include <CellGrid.h>
#include <Coord.h>
#include <vector>
#include <queue>
#include <Vertex.h>


CubeGrid::CubeGrid()
{

}

CubeGrid::CubeGrid(CellGrid& cellGrid, float _cubeSize, float _surfaceLevel, unsigned int minRegionSize) :
    width(cellGrid.width-1),
    height(cellGrid.height-1),
    depth(cellGrid.depth-1),
    cubeSize(_cubeSize),
    surfaceLevel(_surfaceLevel)
{
    // Calculate the coordinates of each cube vertex (i.e. control nodes)
    // First calculate the coordinates of each cells

    controlNodes = new ControlNode**[cellGrid.width];
    for(int i = 0; i < cellGrid.width; i++){
        controlNodes[i] = new ControlNode*[cellGrid.height];
        for(int j = 0; j < cellGrid.height; j++){
            controlNodes[i][j] = new ControlNode[cellGrid.depth];
            for(int k = 0; k < cellGrid.depth; k++){

                vec3d pos(-cellGrid.width*cubeSize/2.f + i*cubeSize + cubeSize/2.f,
                          -cellGrid.height*cubeSize/2.f + j*cubeSize + cubeSize/2.f,
                          -cellGrid.depth*cubeSize/2.f + k*cubeSize + cubeSize/2.f);

                controlNodes[i][j][k] = ControlNode(pos, cellGrid.cells[i][j][k], cellGrid.cells[i][j][k] > surfaceLevel);
            }
        }
    }

    if(minRegionSize > 0)
        ignoreSmallRegions(cellGrid, minRegionSize);
}

void CubeGrid::ignoreSmallRegions(CellGrid& cellGrid, unsigned int minNodeCount)
{
    // Since some small region can still be connected to bigger ones, a similar
    // process on the calculated edge vertices will be necessary.
    // This step though helps to reduce the final number of vertices

    // floodfill algorithm : keep in memory visited vertices during process
    bool ***visited = new bool**[cellGrid.width];
    for(int i = 0; i < cellGrid.width; i++){
        visited[i] = new bool*[cellGrid.height];
        for(int j = 0; j < cellGrid.height; j++){
            visited[i][j] = new bool[cellGrid.depth];
            for(int k = 0; k < cellGrid.depth; k++){
                visited[i][j][k] = false;
            }
        }
    }

    vector<Coord> regionNodes;

    for(int i = 0; i < cellGrid.width; i++){
        for(int j = 0; j < cellGrid.height; j++){
            for(int k = 0; k < cellGrid.depth; k++){
                if(controlNodes[i][j][k].active && !visited[i][j][k]){
                    Coord startCoord(i, j, k);
                    detectRegionNodes(cellGrid, startCoord, visited, regionNodes);

                    if(regionNodes.size() < minNodeCount){
                        for(auto it = regionNodes.begin(); it != regionNodes.end(); ++it){
                            Coord& c = *it;
                            controlNodes[c.i][c.j][c.k].active = false;
                        }
                    }

                    regionNodes.clear();
                }
            }
        }
    }

    for(int i = 0; i < cellGrid.width; i++){
        for(int j = 0; j < cellGrid.height; j++){
            delete[] visited[i][j];
        }
        delete[] visited[i];
    }
    delete[] visited;
}

void CubeGrid::detectRegionNodes(CellGrid& cellGrid, Coord startCoord, bool ***visited, vector<Coord>& regionNodes)
{
    // Floodfill implementation to detect groups of active nodes

    queue<Coord> q;

    visited[startCoord.i][startCoord.j][startCoord.k] = true;

    regionNodes.push_back(startCoord);
    q.push(regionNodes.back());

    Coord c;

    while(!q.empty()){
        c = q.front();
        q.pop();

        for(int i = c.i - 1; i <= c.i + 1; i++){
            for(int j = c.j - 1; j <= c.j + 1; j++){
                for(int k = c.k - 1; k <= c.k + 1; k++){

                    if(i >= 0 && i < cellGrid.width && j >= 0 && j < cellGrid.height && k >= 0 && k < cellGrid.depth){
                        if(i != c.i || j != c.j || k != c.k){
                            if(i == c.i || j == c.j || k == c.k){
                                if(controlNodes[i][j][k].active && !visited[i][j][k]){

                                    visited[i][j][k] = true;
                                    regionNodes.push_back(Coord(i, j, k));
                                    q.push(regionNodes.back());

                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void CubeGrid::marchCubes(vector<Vertex>& vertices)
{
    // create each cubes

    // bordering is a 3 bit number representing the relative position
    // of a cube along the borders of the grid
    int bordering;

    cubes = new Cube**[width];
    for(int i = 0; i < width; i++){
        cubes[i] = new Cube*[height];
        for(int j = 0; j < height; j++){
            cubes[i][j] = new Cube[depth];
            for(int k = 0; k < depth; k++){
                ControlNode **cubeControlNodes = new ControlNode*[8];

                // add the control nodes of the cube

                // front face vertices
                cubeControlNodes[0] = &controlNodes[i][j][k];
                cubeControlNodes[1] = &controlNodes[i][j+1][k];
                cubeControlNodes[2] = &controlNodes[i+1][j+1][k];
                cubeControlNodes[3] = &controlNodes[i+1][j][k];
                // back face, directly opposite to the previous one
                cubeControlNodes[4] = &controlNodes[i][j][k+1];
                cubeControlNodes[5] = &controlNodes[i][j+1][k+1];
                cubeControlNodes[6] = &controlNodes[i+1][j+1][k+1];
                cubeControlNodes[7] = &controlNodes[i+1][j][k+1];

                bordering = 0;
                if(i == width-1) bordering |= 1;
                if(j == height-1) bordering |= 2;
                if(k == depth-1) bordering |= 4;

                cubes[i][j][k] = Cube(cubeControlNodes, bordering);
                Cube& cube = cubes[i][j][k];
                // save process by calculating vertices of cubes that have at least
                // one vertex
                if(cube.configuration != 0 && cube.configuration != 255){
                    cube.createVertices(vertices, surfaceLevel);
                }

            }
        }
    }

    // only necessary vertices were calculated for each cubes
    // we can determine 3/4 of the edge of each cubes from the surrounding ones
    // therefore having cubes sharing vertices

    for(int i = 0; i < width; i++){
        for(int j = 0; j < height; j++){
            for(int k = 0; k < depth; k++){

                Cube& cube = cubes[i][j][k];

                if(cube.configuration == 0 || cube.configuration == 255)
                    continue;

                if(i > 0){
                    cubes[i-1][j][k].edgeNodes[2] = cube.edgeNodes[0];
                    cubes[i-1][j][k].edgeNodes[11] = cube.edgeNodes[8];
                    if(j > 0)
                        cubes[i-1][j-1][k].edgeNodes[10] = cube.edgeNodes[8];
                }
                if(j > 0){
                    cubes[i][j-1][k].edgeNodes[1] = cube.edgeNodes[3];
                    cubes[i][j-1][k].edgeNodes[9] = cube.edgeNodes[8];
                    if(k > 0)
                        cubes[i][j-1][k-1].edgeNodes[5] = cube.edgeNodes[3];
                }
                if(k > 0){
                    cubes[i][j][k-1].edgeNodes[7] = cube.edgeNodes[3];
                    cubes[i][j][k-1].edgeNodes[4] = cube.edgeNodes[0];
                    if(i > 0)
                        cubes[i-1][j][k-1].edgeNodes[6] = cube.edgeNodes[0];
                }

                if(cube.bordering != 0){
                    if((cube.bordering & 1) != 0){
                        if(k > 0) cubes[i][j][k-1].edgeNodes[6] = cube.edgeNodes[2];
                        if(j > 0) cubes[i][j-1][k].edgeNodes[10] = cube.edgeNodes[11];
                    }
                    if((cube.bordering & 2) != 0){
                        if(k > 0) cubes[i][j][k-1].edgeNodes[5] = cube.edgeNodes[1];
                        if(i > 0) cubes[i-1][j][k].edgeNodes[10] = cube.edgeNodes[9];
                    }
                    if((cube.bordering & 4) != 0){
                        if(i > 0) cubes[i-1][j][k].edgeNodes[6] = cube.edgeNodes[4];
                        if(j > 0) cubes[i][j-1][k].edgeNodes[5] = cube.edgeNodes[7];
                    }
                }
            }
        }
    }
}

void CubeGrid::clear(){

    // free memory from all arrays

    for(int i = 0; i < width; i++){
        for(int j = 0; j < height; j++){
            for(int k = 0; k < depth; k++){
                delete[] cubes[i][j][k].edgeNodes;
                delete[] cubes[i][j][k].controlNodes;
            }
            delete[] cubes[i][j];
            delete[] controlNodes[i][j];
        }
        delete cubes[i];
        delete[] controlNodes[i];
    }
    delete[] cubes;
    delete[] controlNodes;
}

CubeGrid::~CubeGrid()
{

}

