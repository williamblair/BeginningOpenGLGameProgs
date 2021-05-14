#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include <iostream>
#include <memory>

#include <Entity.h>
#include <Vec3.h>

class Player : public Entity
{
public:

    Player(GameWorld* const world);
    virtual ~Player();

    virtual EntityType GetType() const { return EntityType::PLAYER; }
    
    virtual Collider* GetCollider() { return collider.get(); }

    Vec3 GetPosition() const { return position; }
    void SetPosition(const Vec3& pos) {
        position = pos;
    }
    
    Vec3 GetVelocity() const { return velocity; }

    float GetYaw() const { return yaw; }
    float GetPitch() const { return pitch; }
    void SetYaw(const float yaw) { this->yaw = yaw; }
    void SetPitch(const float pitch) { this->pitch = pitch; }

    void AddYaw(const float val);
    void AddPitch(const float pitch);
    void MoveForward(const float speed);

private:

    Vec3 position;
    Vec3 velocity;
    
    float yaw;
    float pitch;
    std::unique_ptr<Collider> collider;

    virtual void OnPrepare(float dt);
    virtual void OnRender();
    virtual void OnPostRender();
    virtual bool OnInitialize();
    virtual void OnShutdown();
    virtual void OnCollision(Entity* entity);
    
    inline float deg2rad(const float deg)
    {
        return deg * 3.1415926535 / 180.0f;
    }
};

#endif // PLAYER_H_INCLUDED
