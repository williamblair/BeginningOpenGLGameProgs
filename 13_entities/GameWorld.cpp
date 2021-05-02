#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <cassert>
#include <GameWorld.h>
#include <Ogro.h>
#include <HeightMap.h>
#include <Tree.h>

#define DBG_ASSERT(cond) assert(cond)
//#define DBG_ASSERT(cond)

GameWorld::GameWorld() :
    entities(std::list<Entity*>()),
    heightMap(nullptr),
    lastSpawnTime(0),
    currentTime(0),
    viewMat(nullptr),
    projMat(nullptr)
{

}

GameWorld::~GameWorld()
{
    for (auto& entity : entities)
    {
        delete entity;
        entity = nullptr;
    }

    entities.clear();
}

bool GameWorld::Init()
{
    srand(time(0));

    heightMap = (HeightMap*)SpawnEntity(EntityType::LANDSCAPE);
    Vec3 landscapePos(0.0f, -5.0f, 0.0f);
    heightMap->SetPosition(landscapePos);

    for (int i = 0; i < MAX_ENEMY_COUNT; ++i)
    {
        Entity* newEntity = SpawnEntity(EntityType::OGRO);
        //newEntity->SetPosition(getRandomPosition());
        Vec3 pos(1.0f, 0.0f, -2.0f);
        pos = getRandomPosition();
        pos.y -= 5.0f; // to account for terrain shift down by 5 above
        newEntity->SetPosition(pos);
    }

    for (int i = 0; i < TREE_COUNT; ++i)
    {
        Entity* newEntity = SpawnEntity(EntityType::TREE);
        if (newEntity == nullptr) continue;
        Vec3 pos(0.0f, -1.0f, 0.0f);
        while (pos.y < 1.0f)
        {
            pos = getRandomPosition();
        }
        pos.y -= 5.0f; // to account for terrain shift down by 5 above
        newEntity->SetPosition(pos);
    }

    // TODO
    SpawnEntity(EntityType::PLAYER);
    // getPlayer()->setPosition()...

    return true;
}

void GameWorld::Update(float dt)
{
    currentTime += dt;

    for (Entity* entity : entities)
    {
        entity->Prepare(dt);
    }

    // TODO - collision

    clearDeadEntities();

    // TODO - respawn enemy entities

    // TODO - mouse/player movement input
}

void GameWorld::Render()
{
    DBG_ASSERT(viewMat != nullptr);
    DBG_ASSERT(projMat != nullptr);

    for (Entity* entity : entities)
    {
        Vec3 pos = entity->GetPosition();
        // TODO - add collider null test
        if (entity->GetType() == EntityType::LANDSCAPE)
        {
            entity->Render();
            entity->PostRender();
        }
        // TODO - sphere in frustum test
        //else if ()
        else
        {
            entity->Render();
            entity->PostRender();
        }
    }
}

Entity* GameWorld::SpawnEntity(EntityType et)
{
    Entity* newEntity = nullptr;
    bool initialize = true;
    switch (et)
    {
    case EntityType::OGRO:
        // For OGROs, we can use old ones
        // TODO
        newEntity = new Ogro(this);
        lastSpawnTime = currentTime;

        break;
    case EntityType::LANDSCAPE:
        newEntity = new HeightMap(this);
        lastSpawnTime = currentTime;

        break;
    case EntityType::TREE:
        newEntity = new Tree(this);
        lastSpawnTime = currentTime;
        break;
    default:
        std::cout << __func__ << ": unhandled entity type: "
                  << EntityTypeToString(et) << std::endl;
        return newEntity;
    }

    if (initialize && !newEntity->Init())
    {
        delete newEntity;
        throw std::runtime_error("Failed to initialize entity");
    }

    // TODO - entity get collider

    registerEntity(newEntity);

    return newEntity;
}

void GameWorld::registerEntity(Entity* entity)
{
    if (std::find(entities.begin(), entities.end(), entity) != entities.end())
    {
        return;
    }
    entities.push_back(entity);
}

void GameWorld::clearDeadEntities()
{
    for (auto entity = entities.begin(); entity != entities.end(); )
    {
        if (!(*entity)->CanBeRemoved())
        {
            ++entity;
            continue;
        }

        // TODO - unregister collider
        delete (*entity);
        entity = entities.erase(entity);
    }
}

Vec3 GameWorld::getRandomPosition()
{
    // TODO - base off of landscape size
    //float randX = rand() / (float(RAND_MAX) + 1);
    //float randZ = rand() / (float(RAND_MAX) + 1);
    //float randX = 0.0f;
    //float randZ = -2.0f;
    //float randY = 0.0f;
    float randX = heightMap->GetMinX() + (rand() % int(heightMap->GetMaxX() - heightMap->GetMinX()));
    float randZ = heightMap->GetMinZ() + (rand() % int(heightMap->GetMaxZ() - heightMap->GetMinZ()));
    float randY = heightMap->GetHeightAt(randX, randZ);

    std::cout << "rand x, y, z: " << randX << ", " << randY << ", " << randZ << std::endl;
    return Vec3(randX, randY, randZ);
}
