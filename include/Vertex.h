#ifndef VERTEX_H
#define VERTEX_H

#include <vec3d.h>


struct Vertex
{
    vec3d pos;
    vec3d normal;
    Vertex(vec3d _pos);
};

#endif // VERTEX_H
