#ifndef GAME_WORLD_H_INCLUDED
#define GAME_WORLD_H_INCLUDED

#include <list>
#include <Vec3.h>
#include <Mat4.h>
#include <Entity.h>
#include <EntityType.h>

// forward declaration
class HeightMap;

class GameWorld
{
public:

    // TODO - add keyboard/mouse interface or other input
    GameWorld();
    ~GameWorld();

    bool Init();
    void Update(float dt);
    void Render();

    Entity* SpawnEntity(EntityType et);

    void SetViewMatrix(Mat4* view) { viewMat = view; }
    Mat4* GetViewMatrix() const { return viewMat; }

    void SetProjectionMatrix(Mat4* proj) { projMat = proj; }
    Mat4* GetProjectionMatrix() const { return projMat; } 

    HeightMap* GetHeightMap() { return heightMap; }
    

private:

    std::list<Entity*> entities;
    std::list<Collider*> colliders;
    HeightMap* heightMap;

    static const int MAX_ENEMY_COUNT = 60;
    static const int TREE_COUNT = 30;

    float lastSpawnTime;
    float currentTime;

    Mat4* viewMat;
    Mat4* projMat;

    void registerEntity(Entity* entity);
    void registerCollider(Collider* collider);
    void unregisterCollider(Collider* collider);
    void clearDeadEntities();
    Vec3 getRandomPosition();
};

#endif // GAME_WORLD_H_INCLUDED
