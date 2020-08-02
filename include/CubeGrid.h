#ifndef CUBEGRID_H
#define CUBEGRID_H

#include <Cube.h>
#include <ControlNode.h>
#include <CellGrid.h>
#include <vector>
#include <vec3d.h>
#include <Coord.h>
#include <Vertex.h>

// class generating and storing the 3D grid of cubes

class CubeGrid
{
    public:
        int width = 0;
        int height = 0;
        int depth = 0;
        float cubeSize;
        float surfaceLevel;
        ControlNode ***controlNodes;
        Cube ***cubes;

        CubeGrid();
        // Generates the controls nodes from the cell grid and filters small regions
        void generateGrid(CellGrid& cellGrid, float _cubeSize, float _surfaceLevel, unsigned int minRegionSize);
        // Generates each cubes and their edge vertices
        void marchCubes(vector<Vertex>& vertices);
        void clear();
        virtual ~CubeGrid();

    protected:

    private:
        // Set as not active all nodes belonging to small regions we don't want to see.
        void ignoreSmallRegions(CellGrid& cellGrid, unsigned int minNodeCount);
        // Utility method used by the ignoreSmallRegions methode : apply floodfill to get
        // nodes per regions from a starting node
        void detectRegionNodes(CellGrid& cellGrid, Coord startCoord, bool ***visited, vector<Coord>& regionNodes);
};

#endif // CUBEGRID_H

