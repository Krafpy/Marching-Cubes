#ifndef VEC3D_H
#define VEC3D_H

#include <iostream>

// basic 3D vector class for common vector operations

class vec3d
{
    public:
        float x, y, z;

        vec3d();
        vec3d(float, float, float);
        virtual ~vec3d();
        static vec3d cross(vec3d v1, vec3d v2);
        static float dot(vec3d v1, vec3d v2);
        static float angleBetween(vec3d v1, vec3d v2);
        float length();
        static float distance(vec3d p1, vec3d p2);
        void normalize();

        vec3d& operator += (const vec3d& v);
        vec3d& operator -= (const vec3d& v);
        vec3d& operator *= (const float c);
        vec3d& operator /= (const float c);

    protected:

    private:
};

std::ostream& operator << (std::ostream& out, const vec3d& v);
bool operator == (const vec3d& v1, const vec3d& v2);
bool operator != (const vec3d& v1, const vec3d& v2);
bool operator < (const vec3d& v1, const vec3d& v2); // for eventual use as map key
vec3d operator + (const vec3d& v1, const vec3d& v2);
vec3d operator - (const vec3d& v1, const vec3d& v2);
vec3d operator * (const vec3d& v, const float c);
vec3d operator / (const vec3d& v, const float c);
vec3d operator * (const float c, const vec3d& v);
vec3d operator / (const float c, const vec3d& v);

#endif // VEC3D_H
