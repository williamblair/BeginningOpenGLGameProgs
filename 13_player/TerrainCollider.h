#ifndef TERRAIN_COLLIDER_H_INCLUDED
#define TERRAIN_COLLIDER_H_INCLUDED

#include <Collider.h>

// forward declaration
class HeightMap;

class TerrainCollider : public Collider
{
public:
    TerrainCollider(Entity* entity, HeightMap* heightMap);

    float GetRadius() const { return 0.0f; }
    void SetRadius(const float radius) { (void)radius; }

private:
    bool collideWith(const Collider* collider);
    HeightMap* heightMap;
};

#endif // TERRAIN_COLLIDER_H_INCLUDED

