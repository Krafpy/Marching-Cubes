# Marching-Cubes
This project is inspired from Sebastian Lague's [marching squares](https://www.youtube.com/playlist?list=PLFt_AvWsXl0eZgMK_DT5_biRkWXftAOf9) and [marching cubes](https://www.youtube.com/watch?v=M3iI2l0ltbE) videos, as well as Coding Train's *Coding in the Cabana* video on this topic.
Feel free to add any contribution.

![Screenshot](/screenshots/screenshot5.PNG)

## Dependencies
This C++ code uses [GLFW3](https://www.glfw.org/) and [GLEW](http://glew.sourceforge.net/) libraries for the 3D rendering. The noise functions come from [FastNoise](https://github.com/Auburn/FastNoise) library by Auburn. This project was originally developed on CodeBlocks IDE using GNU GCC compiler.

## Controls
* Drag the mouse to rotate around the generated shape;
* `space` to generate a new random shape;
* `A` to show/hide axes (red for x, yellow for y, blue for z);
* `Q` to show/hide wireframe box.

## Improvements ideas
- [ ] Complete the `CellGrid::fillGrid()` method to allow more advanced patterns (e.g. floored terrain, terracing);
- [ ] Use compute shaders to generate the noise and the cubes/vertices;
- [ ] Add UI to configure the noise function

## Project ideas
* Continuous terrain generator;
* Simulate flocking behaviour with boids around the generated terrain.

## Used resources
* http://paulbourke.net/geometry/polygonise/
* https://developer.nvidia.com/gpugems/gpugems3/part-i-geometry/chapter-1-generating-complex-procedural-terrains-using-gpu
* https://www.khronos.org/opengl/wiki/VBO_-_just_examples
