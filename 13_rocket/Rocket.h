#ifndef ROCKET_H_INCLUDED
#define ROCKET_H_INCLUDED

#include <Entity.h>
#include <EntityType.h>
#include <TargaImage.h>
#include <Shader.h>

// forward declaration
class Md2Model;

class Rocket : public Entity
{
public:
    Rocket(GameWorld* const gameWorld);
    virtual ~Rocket();
    
    EntityType GetType() const { return EntityType::ROCKET; }
    
    Vec3 GetPosition() const { return position; }
    void SetPosition(const Vec3& pos) { position = pos; }
    
    float GetYaw() const { return yaw; }
    float GetPitch() const { return pitch; }
    void SetYaw(const float yaw) { this->yaw = yaw; }
    void SetPitch(const float pitch) { this->pitch = pitch; }
    
    Collider* GetCollider() { return collider; }

private:

    Vec3 position;
    float yaw;
    float pitch;
    Collider* collider;
    Md2Model* model;
    unsigned int rocketTexId;
    Targa::Image rocketTexture;
    
    static bool shaderLoaded;
    static Shader shader;
    
    virtual void OnPrepare(float dt);
    virtual void OnRender();
    virtual void OnPostRender();
    virtual bool OnInitialize();
    virtual void OnShutdown();
    virtual void OnCollision(Entity* other);
    
    inline float deg2rad(const float deg)
    {
        return deg * 3.1415926535 / 180.0f;
    }
};

#endif // ROCKET_H_INCLUDED
