#ifndef GAME_WORLD_H_INCLUDED
#define GAME_WORLD_H_INCLUDED

#include <list>
#include <Vec3.h>
#include <Entity.h>
#include <EntityType.h>

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

private:

    std::list<Entity*> entities;

    static const int MAX_ENEMY_COUNT = 15;
    static const int TREE_COUNT = 20;

    float lastSpawnTime;
    float currentTime;

    void registerEntity(Entity* entity);
    void clearDeadEntities();
    Vec3 getRandomPosition();
};

#endif // GAME_WORLD_H_INCLUDED
