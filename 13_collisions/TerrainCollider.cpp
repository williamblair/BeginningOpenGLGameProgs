#include <TerrainCollider.h>
#include <Entity.h>
#include <HeightMap.h>
#include <Vec3.h>

TerrainCollider::TerrainCollider(Entity* entity, HeightMap* heightMap) :
    Collider(entity),
    heightMap(heightMap)
{
}

bool TerrainCollider::collideWith(const Collider* collider)
{
    // if the collider falls below the terrain, move it back up above the terrain

    Vec3 pos = collider->GetEntity()->GetPosition();
    float height = heightMap->GetHeightAt(pos.x, pos.z);
    float radius = collider->GetRadius();

    if (pos.y < (height + radius))
    {
        return true;
    }

    return false;
}
