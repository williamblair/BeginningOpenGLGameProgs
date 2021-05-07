#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

#include <Vec3.h>
#include <Mat4.h>

// forward declaration
class Entity;

class Camera
{
public:
    Camera();
    Camera(Vec3& position);
    ~Camera();
    
    Mat4* GetViewMatrix() { return &viewMatrix; }

private:
    Vec3 position;
    Vec3 lookAtPos;
    Vec3 right;
    Vec3 forward;
    Vec3 up;
    
    Mat4 viewMatrix;
    
    Entity* attachedEntity;
    
    void UpdateMatrix();
};

#endif // CAMERA_H_INCLUDED
