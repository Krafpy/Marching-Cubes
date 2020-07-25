#ifndef CELLGRID_H
#define CELLGRID_H

#include <FastNoise.h>

using namespace std;


class CellGrid
{
    public:
        float ***cells;
        int width = 0;
        int height = 0;
        int depth = 0;

        CellGrid();
        CellGrid(int _width, int _height, int _depth);

        // fill the grid with 3D noise values
        void fillGrid(FastNoise& noise, int octaves, float lacunarity, float persistance, float scale);
        virtual ~CellGrid();

    protected:

    private:
};

#endif // CELLGRID_H
