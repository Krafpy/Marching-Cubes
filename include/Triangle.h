#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <vec3d.h>


struct Triangle
{
    unsigned int a, b, c;
    vec3d normal;
    Triangle(unsigned int _a, unsigned int _b, unsigned int _c);
};

#endif // TRIANGLE_H
