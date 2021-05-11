#ifndef SPHERE_COLLIDER_H_INCLUDED
#define SPHERE_COLLIDER_H_INCLUDED

#include <Collider.h>

class SphereCollider : public Collider
{
public:
    SphereCollider(Entity* entity, float radius);
    virtual ~SphereCollider();

    virtual float GetRadius() const { return radius; }
    void SetRadius(const float radius) { this->radius = radius; }

private:
    float radius;
    bool collideWith(const Collider* collider);
};

#endif // SPHERE_COLLIDER_H_INCLUDED

