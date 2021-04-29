#include <Entity.h>

Entity::Entity(GameWorld* gameWorld) :
    canBeRemoved(false),
    gameWorld(gameWorld)
{}

Entity::~Entity()
{}

void Entity::Prepare(float dt)
{}

void Entity::Render()
{
    OnRender();
}

void Entity::PostRender()
{
    OnPostRender();
}

bool Entity::Init()
{
    return OnInitialize();
}

void Entity::Shutdown()
{
    OnShutdown();
}

void Entity::Destroy()
{
    canBeRemoved = true;
}

void Entity::Collide(Entity* collider)
{
    OnCollision(collider);
}

