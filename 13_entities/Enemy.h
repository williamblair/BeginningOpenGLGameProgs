#ifndef ENEMY_H_INCLUDED
#define ENEMY_H_INCLUDED

#include <Vec3.h>
#include <Entity.h>

/* Forward Declarations */
class GameWorld;

class Enemy : public Entity
{
public:

    Enemy(GameWorld* const gameWorld);
    virtual ~Enemy();

    virtual void OnCollision(Entity* collider);

    Vec3 GetPosition() const { return position; }
    void SetPosition(const Vec3& pos) { position = pos; }
    Vec3 GetVelocity() const { return velocity; }

    float GetYaw() const { return yaw; }
    float GetPitch() const { return 0.0f; }
    void SetYaw(float yaw) { this->yaw = yaw; }
    void SetPitch(float) {}

    bool IsDead() const { return isDead; }
    void Kill() { isDead = true; OnKill(); }

protected:
    Vec3 position;
    Vec3 velocity;

    float yaw;

    bool isDead;

private:
    virtual void OnKill() = 0;
    virtual void OnResurrection() = 0;
};

#endif // ENEMY_H_INCLUDED
