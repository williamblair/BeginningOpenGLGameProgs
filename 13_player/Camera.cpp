#include <Camera.h>
#include <Entity.h>

Camera::Camera() :
    position(0.0f, 0.0f, 0.0f),
    lookAtPos(0.0f, 0.0f, 1.0f),
    right(1.0f, 0.0f, 0.0f),
    forward(0.0f, 0.0f, 1.0f),
    up(0.0f, 1.0f, 0.0f),
    yaw(0.0f),
    pitch(0.0f),
    viewMatrix(Mat4()),
    attachedEntity(nullptr)
{
    UpdateMatrix();
}

Camera::Camera(Vec3& position) :
    position(position),
    lookAtPos(0.0f, 0.0f, 1.0f),
    right(1.0f, 0.0f, 0.0f),
    forward(0.0f, 0.0f, 1.0f),
    up(0.0f, 1.0f, 0.0f),
    yaw(0.0f),
    pitch(0.0f),
    viewMatrix(Mat4()),
    attachedEntity(nullptr)
{
    UpdateMatrix();
}

Camera::~Camera()
{
}

void Camera::AddYaw(const float degrees)
{
    yaw += degrees;
    if (yaw >= 360.0f)
    {
        yaw -= 360.0f;
    }
    else if (yaw < 0.0f)
    {
        yaw += 360.0f;
    }
    
    float yawRad = deg2rad(yaw);
    float sinYaw = sinf(yawRad);
    float cosYaw = cosf(yawRad);
    
    right.x = cosYaw * len(right);
    right.z = sinYaw * len(right);
    
    forward.x = -sinYaw * len(forward);
    forward.z = cosYaw * len(forward);
}

void Camera::AddPitch(const float degrees)
{
}

void Camera::MoveForward(const float speed)
{
    float yawRad = deg2rad(yaw);
    float cosYaw = cosf(yawRad);
    float sinYaw = sinf(yawRad);
    
    position.x += cosYaw * speed;
    position.z += sinYaw * speed;
}

void Camera::UpdateMatrix()
{
    if (attachedEntity != nullptr)
    {
        Vec3 newPos = attachedEntity->GetPosition();
        SetPosition(newPos);
        yaw = attachedEntity->GetYaw();
        pitch = attachedEntity->GetPitch();
    }
    
    float yawRad = deg2rad(yaw);
    float pitchRad = deg2rad(pitch);
    float cosYaw = cosf(yawRad);
    float sinYaw = sinf(yawRad);
    float sinPitch = sinf(pitchRad);
    
    // calculate target position based on new position
    lookAtPos.x = position.x + cosYaw;
    lookAtPos.y = position.y + sinPitch;
    lookAtPos.z = position.z + sinYaw;

    viewMatrix = lookAt(position,
                        lookAtPos,
                        up);
}

