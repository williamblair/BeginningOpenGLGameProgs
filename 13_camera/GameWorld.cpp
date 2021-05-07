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
    colliders(std::list<Collider*>()),
    heightMap(nullptr),
    camera(nullptr),
    mouseInterface(nullptr),
    lastSpawnTime(0),
    currentTime(0),
    projMat(nullptr),
    mouseRelX(0.0f),
    mouseRelY(0.0f),
    screenWidth(0.0f),
    screenHeight(0.0f)
{
}

GameWorld::~GameWorld()
{
    for (auto& entity : entities)
    {
        delete entity;
        entity = nullptr;
    }
    
    if (camera != nullptr)
    {
        delete camera;
        camera = nullptr;
    }

    entities.clear();
    colliders.clear();
}

bool GameWorld::Init()
{
    srand(time(0));

    Vec3 cameraPos(0.0f, 10.0f, 0.0f);
    camera = new Camera();
    if (camera == nullptr) {
        std::cout << __func__ << ": Failed to allocate camera" << std::endl;
        return false;
    }
    camera->SetPosition(cameraPos);

    heightMap = (HeightMap*)SpawnEntity(EntityType::LANDSCAPE);

    for (int i = 0; i < MAX_ENEMY_COUNT; ++i)
    {
        Entity* newEntity = SpawnEntity(EntityType::OGRO);
        Vec3 pos(1.0f, 0.0f, -2.0f);
        pos = getRandomPosition();
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

    Collider::UpdateColliders(colliders);
    clearDeadEntities();

    // TODO - respawn enemy entities

    // Last mouse positions stored int list;
    // Each frame new relative position added and oldest is removed
    // weighted mouse position is then calcuated; newest position has most weight
    // then averaged to produce new relative mouse pos.
    static std::list<std::pair<float, float>> mousePosHistory;
    
    int x, y;
    mouseInterface->GetMousePos(x, y);
    mouseInterface->ShowCursor(false); // TODO - this doesn't do anything
    
    // Add current mouse position - starting position (so pos is relative)
    mousePosHistory.push_front(std::make_pair(float(x) - (screenWidth / 2),
                                              float(y) - (screenHeight / 2)));
    if (mousePosHistory.size() > 10) {
        mousePosHistory.pop_back();
    }
    
    mouseRelX = 0.0f;
    mouseRelY = 0.0f;
    float weight = 1.0f;
    
    // calculate weighted average
    for (auto& m : mousePosHistory)
    {
        mouseRelX += m.first * weight;
        mouseRelY += m.second * weight;
        
        weight *= 0.5f;
    }
    mouseRelX /= 10.0f;
    mouseRelY /= 10.0f;
    
    // put mouse in middle of screen
    mouseInterface->SetMousePos(int(screenWidth / 2), int(screenHeight / 2)); // TODO - doesn't do anything
}

void GameWorld::Render()
{
    //DBG_ASSERT(viewMat != nullptr);
    DBG_ASSERT(GetViewMatrix() != nullptr);
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

    if (newEntity->GetCollider())
    {
        registerCollider(newEntity->GetCollider());
    }

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

void GameWorld::registerCollider(Collider* collider)
{
    if (std::find(colliders.begin(), colliders.end(), collider) != colliders.end())
    {
        return;
    }
    colliders.push_back(collider);
}

void GameWorld::unregisterCollider(Collider* collider)
{
    auto iter = std::find(colliders.begin(), colliders.end(), collider);
    if (iter == colliders.end())
    {
        return;
    }
    colliders.erase(iter);
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

        unregisterCollider((*entity)->GetCollider());
        delete (*entity);
        entity = entities.erase(entity);
    }
}

Vec3 GameWorld::getRandomPosition()
{
    float randX = heightMap->GetMinX() + (rand() % int(heightMap->GetMaxX() - heightMap->GetMinX()));
    float randZ = heightMap->GetMinZ() + (rand() % int(heightMap->GetMaxZ() - heightMap->GetMinZ()));
    float randY = heightMap->GetHeightAt(randX, randZ);

    return Vec3(randX, randY, randZ);
}
