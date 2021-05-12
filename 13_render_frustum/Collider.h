#ifndef COLLIDER_H_INCLUDED
#define COLLIDER_H_INCLUDED

#include <list>

// forward declaration
class Entity;

class Collider
{
public:

    Collider(Entity* entity);
    virtual ~Collider() { entity = nullptr; }

    virtual float GetRadius() const = 0;
    virtual void  SetRadius(const float radius) = 0;
    
    static void UpdateColliders(std::list<Collider*>& colliders);

    Entity* GetEntity() const { return entity; }

private:

    virtual bool collideWith(const Collider* collider) = 0;

    Entity* entity;
};

#endif // COLLIDER_H_INCLUDED

