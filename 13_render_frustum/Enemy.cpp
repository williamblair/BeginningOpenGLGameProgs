#include <Enemy.h>
#include <SphereCollider.h>
#include <iostream>

Enemy::Enemy(GameWorld* const gameWorld) :
    Entity(gameWorld),
    position(Vec3()),
    velocity(Vec3()),
    yaw(0.0f),
    isDead(false)
{
    collider = new SphereCollider(this, 0.0f);
}

Enemy::~Enemy()
{
    delete collider;
    collider = nullptr;
}

void Enemy::OnCollision(Entity* collider)
{
    if (collider->GetType() == EntityType::ROCKET && !isDead)
    {
        // TODO - increase player score
        std::cout << "Killing enemy based on rocket collision" << std::endl;
        Kill();
    }
}

