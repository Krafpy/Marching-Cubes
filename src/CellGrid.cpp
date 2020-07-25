#include "CellGrid.h"

#include <FastNoise.h>
#include <vec3d.h>
#include <math.h>


using namespace std;


CellGrid::CellGrid()
{

}

CellGrid::CellGrid(int _width, int _height, int _depth) : width(_width), height(_height), depth(_depth)
{
    cells = new float**[width];
    for(int i = 0; i < width; i++){
        cells[i] = new float*[height];
        for(int j = 0; j < height; j++){
            cells[i][j] = new float[depth];
        }
    }
}

void CellGrid::fillGrid(FastNoise& noise, int octaves, float lacunarity, float persistance, float scale)
{
    float noiseX, noiseY, noiseZ;
    float posX, posY, posZ;
    float noiseValue;
    float weight;
    float frequency;


    for(int i = 0; i < width; i++){
        for(int j = 0; j < height; j++){
            for(int k = 0; k < depth; k++){

                noiseValue = 0.;

                for(int u = 0; u < octaves; u++){
                    frequency = pow(lacunarity, u) * scale;
                    noiseX = (float)i * frequency;
                    noiseY = (float)j * frequency;
                    noiseZ = (float)k * frequency;
                    noiseValue += noise.GetNoise(noiseX, noiseY, noiseZ) * pow(persistance, u);
                }


                // TODO : add more parameters to allow configurable patterns such as tesselation
                // commented hardcode example below

                /*y = (float)j * 0.1f - 0.1f * height/2.f;
                noiseValue = -(y + 1.f) + noiseValue * 1.9f + fmod(y, 0.9f) * 1.f;*/

                // ensure closed edges, comment the code bellow and just assign the
                // noise value to the cell to remove them

                posX = fabs((float)i - (float)width / 2.f + 0.5f) * 2.f;
                posY = fabs((float)j - (float)height / 2.f + 0.5f) * 2.f;
                posZ = fabs((float)k - (float)depth / 2.f + 0.5f) * 2.f;

                posX += -width + 2.f;
                posY += -height + 2.f;
                posZ += -depth + 2.f;

                weight = max(posX, max(posY, posZ));

                weight = weight < 0.f ? 0.f : weight > 1.f ? 1.f : weight;

                cells[i][j][k] = noiseValue * (1 - weight) - 100.f * weight;
            }
        }
    }
}

CellGrid::~CellGrid()
{

}
