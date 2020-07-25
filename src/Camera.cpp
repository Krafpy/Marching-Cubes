#include "Camera.h"

#include <vec3d.h>
#include <math.h>

#define PI 3.141592f


Camera::Camera()
{

}

Camera::Camera(vec3d _pos, float _rotSpeed, float _zoomSpeed) : pos(_pos), rotSpeed(_rotSpeed), zoomSpeed(_zoomSpeed)
{
    // setup the camera and calculate start angles and distance

    deviationAngle = vec3d::angleBetween(vec3d(pos.x, 0, pos.z), vec3d(0, 0, -1));
    heightAngle = vec3d::angleBetween(vec3d(0, pos.y, pos.z), vec3d(0, 0, -1));

    distanceFromCenter = vec3d::distance(pos, vec3d());
}

void Camera::updatePos(float mouseX, float mouseY, float deltaTime)
{
    if(isMoving){
        float dx = (mouseX - prevMouseX) * rotSpeed * deltaTime;
        float dy = (mouseY - prevMouseY) * rotSpeed * deltaTime;

        deviationAngle += dx;
        heightAngle += dy;

        if(deviationAngle < -PI)
            deviationAngle = PI;
        else if (deviationAngle > PI)
            deviationAngle = -PI;

        // constrain camera's height
        if(heightAngle < -PI/2.f && dy < 0.f)
            heightAngle = -PI/2.f + 0.01f;
        else if(heightAngle > PI/2.f && dy > 0.f)
            heightAngle = PI/2.f - 0.01f;

        prevMouseX = mouseX;
        prevMouseY = mouseY;
    } else {
        prevMouseX = mouseX;
        prevMouseY = mouseY;
    }
}

void Camera::updateDistance(float offset, float minDist, float maxDist)
{
    distanceFromCenter += -offset * zoomSpeed;

    if(distanceFromCenter < minDist && offset > 0.f)
        distanceFromCenter = minDist;
    else if (distanceFromCenter > maxDist && offset < 0.f)
        distanceFromCenter = maxDist;
}

void Camera::update()
{
    // calculate eye coordinates with spherical to cartesian
    // coordinates conversion

    pos.x = distanceFromCenter * cos(heightAngle) * sin(deviationAngle);
    pos.y = distanceFromCenter * sin(heightAngle);
    pos.z = -distanceFromCenter * cos(heightAngle) * cos(deviationAngle);
}

void Camera::enableMovement()
{
    isMoving = true;
}

void Camera::disableMovement()
{
    isMoving = false;
}

Camera::~Camera()
{

}
