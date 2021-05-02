#include <SphereCollider.h>
#include <Entity.h>
#include <Vec3.h>
#include <iostream>

SphereCollider::SphereCollider(Entity* entity, float radius) :
    Collider(entity),
    radius(radius)
{
}

SphereCollider::~SphereCollider()
{
}

bool SphereCollider::collideWith(const Collider* collider)
{
    Vec3 entityPos = GetEntity()->GetPosition();
    float entityRadius = GetRadius();

    Vec3 otherPos = collider->GetEntity()->GetPosition();
    float otherRadius = collider->GetRadius();

    // check if spheres overlap
    if (len(entityPos - otherPos) <= (entityRadius + otherRadius))
    {
        return true;
    }

    return false;
}

