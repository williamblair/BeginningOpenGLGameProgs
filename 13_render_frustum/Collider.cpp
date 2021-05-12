#include <Collider.h>
#include <Entity.h>

Collider::Collider(Entity* entity) :
    entity(entity)
{
}

void Collider::UpdateColliders(std::list<Collider*>& colliders)
{
    for (Collider* collider : colliders)
    {
        // skip dead entities
        if (collider->GetEntity()->CanBeRemoved())
        {
            continue;
        }

        for (Collider* other : colliders)
        {
            if (other == collider || other->GetEntity()->CanBeRemoved())
            {
                // testing against ourselves or other is dead
                continue;
            }

            // check for collision
            if (collider->collideWith(other))
            {
                // call both entities collision handlers
                collider->GetEntity()->Collide(other->GetEntity());
                other->GetEntity()->Collide(collider->GetEntity());
            }
        }
    }
}

