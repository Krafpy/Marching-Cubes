#include "vec3d.h"

#include <iostream>
#include <math.h>

vec3d::vec3d(float _x=0, float _y=0, float _z=0) : x(_x), y(_y), z(_z)
{

}

vec3d::vec3d() : x(0), y(0), z(0)
{

}

vec3d::~vec3d()
{

}

float vec3d::dot(vec3d v1, vec3d v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

float vec3d::angleBetween(vec3d v1, vec3d v2)
{
    return acos(dot(v1, v2) / (v1.length() * v2.length()));
}

vec3d vec3d::cross(vec3d v1, vec3d v2)
{
    return vec3d(v1.y*v2.z - v1.z*v2.y,
                 v1.z*v2.x - v1.x*v2.z,
                 v1.x*v2.y - v1.y*v2.x);
}

float vec3d::length()
{
    return sqrt(x*x + y*y + z*z);
}

void vec3d::normalize()
{
    float len = length();
    x /= len;
    y /= len;
    z /= len;
}

float vec3d::distance(vec3d p1, vec3d p2)
{
    return (p1 - p2).length();
}

std::ostream& operator << (std::ostream& out, const vec3d& v)
{
    return out << v.x << ", " << v.y << ", " << v.z;
}

bool operator == (const vec3d& v1, const vec3d& v2)
{
    return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z;
}

bool operator != (const vec3d& v1, const vec3d& v2)
{
    return v1.x != v2.x || v1.y != v2.y || v1.z != v2.z;
}

bool operator < (const vec3d& v1, const vec3d& v2)
{
    return v1.x < v2.x || v1.y < v2.y || v1.z < v2.z;
}

vec3d operator + (const vec3d& v1, const vec3d& v2)
{
    return vec3d(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

vec3d operator - (const vec3d& v1, const vec3d& v2)
{
    return vec3d(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

vec3d operator * (const vec3d& v, const float c)
{
    return vec3d(v.x * c, v.y * c, v.z * c);
}

vec3d operator / (const vec3d& v, const float c)
{
    return vec3d(v.x / c, v.y / c, v.z / c);
}

vec3d operator * (const float c, const vec3d& v)
{
    return v * c;
}

vec3d operator / (const float c, const vec3d& v)
{
    return v / c;
}

vec3d& vec3d::operator += (const vec3d& v)
{
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}

vec3d& vec3d::operator -= (const vec3d& v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
}

vec3d& vec3d::operator *= (const float c)
{
    x *= c;
    y *= c;
    z *= c;
    return *this;
}

vec3d& vec3d::operator /= (const float c)
{
    x /= c;
    y /= c;
    z /= c;
    return *this;
}
