#ifndef CAMERA_H
#define CAMERA_H

#include <vec3d.h>


class Camera
{
    public:
        vec3d pos;
        vec3d center;
        float rotSpeed;
        float zoomSpeed;

        Camera();
        Camera(vec3d _pos, float _rotSpeed, float _zoomSpeed);
        void enableMovement();
        void disableMovement();
        // Calculate camera eye coordinates
        void update();
        // Update spherical angles according to mouse movements
        void updatePos(float mouseX, float mouseY, float deltaTime);
        // Update the distance to the center of the scene
        // (i.e. radius of spherical coordinates)
        void updateDistance(float offset, float minDist, float maxDist);
        virtual ~Camera();

    protected:

    private:
        bool isMoving = false;
        float prevMouseX, prevMouseY;
        float heightAngle, deviationAngle; // resêctively latidude and longtitude
        float distanceFromCenter;
};

#endif // CAMERA_H
