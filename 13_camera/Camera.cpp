#include <Camera.h>
#include <Entity.h>

Camera::Camera() :
    position(0.0f, 0.0f, 0.0f),
    lookAtPos(0.0f, 0.0f, 0.0f),
    right(1.0f, 0.0f, 0.0f),
    forward(0.0f, 0.0f, 1.0f),
    up(0.0f, 1.0f, 0.0f),
    viewMatrix(Mat4()),
    attachedEntity(nullptr)
{
    UpdateMatrix();
}

Camera::Camera(Vec3& position) :
    position(position),
    lookAtPos(0.0f, 0.0f, 0.0f),
    right(1.0f, 0.0f, 0.0f),
    forward(0.0f, 0.0f, 1.0f),
    up(0.0f, 1.0f, 0.0f),
    viewMatrix(Mat4()),
    attachedEntity(nullptr)
{
    UpdateMatrix();
}

Camera::~Camera()
{
}

void Camera::UpdateMatrix()
{
    // TODO
    viewMatrix = lookAt(Vec3(0.0f, 5.0f, -30.0f),
                        Vec3(0.0f, 3.0f, 0.0f),
                        Vec3(0.0f, 1.0f, 0.0f));
}

