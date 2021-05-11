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
    
    void SetPosition(Vec3& position) { 
        this->position = position; 
    }
    
    void AddYaw(const float degrees);
    void AddPitch(const float degrees);
    void MoveForward(const float speed);
    
    Mat4* GetViewMatrix() { UpdateMatrix(); return &viewMatrix; }
    
    void AttachTo(Entity* entity) {
        attachedEntity = entity;
    }

private:
    Vec3 position;
    Vec3 lookAtPos;
    Vec3 right;
    Vec3 forward;
    Vec3 up;
    
    float yaw;
    float pitch;
    
    Mat4 viewMatrix;
    
    Entity* attachedEntity;
    
    inline float deg2rad(const float deg)
    {
        return deg * 3.1415926535f / 180.0f;
    }
    void UpdateMatrix();
};

#endif // CAMERA_H_INCLUDED
