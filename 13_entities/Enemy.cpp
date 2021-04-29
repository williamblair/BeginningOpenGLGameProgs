#include <Enemy.h>

Enemy::Enemy(GameWorld* const gameWorld) :
    Entity(gameWorld),
    position(Vec3()),
    velocity(Vec3()),
    yaw(0.0f),
    isDead(false)
{
    // TODO collider
}

Enemy::~Enemy()
{
    // TODO delete collider
}

void Enemy::OnCollision(Entity* collider)
{
    // TODO
}
