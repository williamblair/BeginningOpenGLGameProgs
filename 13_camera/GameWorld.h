#ifndef GAME_WORLD_H_INCLUDED
#define GAME_WORLD_H_INCLUDED

#include <list>
#include <Vec3.h>
#include <Mat4.h>
#include <Entity.h>
#include <EntityType.h>
#include <Camera.h>
#include <MouseInterface.h>
#include <KeyboardInterface.h>

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

    Mat4* GetViewMatrix() { return camera->GetViewMatrix(); }

    void SetProjectionMatrix(Mat4* proj) { projMat = proj; }
    Mat4* GetProjectionMatrix() const { return projMat; } 
    
    void SetMouseInterface(MouseInterface* mouseInterface) {
        this->mouseInterface = mouseInterface;
    }
    MouseInterface* GetMouseInterface() {
        return mouseInterface;
    }
    
    void SetKeyboardInterface(KeyboardInterface* keyboardInterface) {
        this->keyboardInterface = keyboardInterface;
    }
    KeyboardInterface* GetKeyboardInterface() {
        return keyboardInterface;
    }

    HeightMap* GetHeightMap() { return heightMap; }
    
    void SetScreenWidth(const float width) { 
        screenWidth = width; 
        mouseInterface->SetScreenWidth(width);
        lastMouseX = width / 2.0f;
    }
    void SetScreenHeight(const float height) { 
        screenHeight = height; 
        mouseInterface->SetScreenHeight(height);
        lastMouseY = height / 2.0f;
    }
    
    void GetRelativeMousePos(float& x, float& y) {
        x = mouseRelX;
        y = mouseRelY;
    }

private:

    std::list<Entity*> entities;
    std::list<Collider*> colliders;
    HeightMap* heightMap;
    Camera* camera;
    MouseInterface* mouseInterface;
    KeyboardInterface* keyboardInterface;

    static const int MAX_ENEMY_COUNT = 60;
    static const int TREE_COUNT = 30;

    float lastSpawnTime;
    float currentTime;
    
    Mat4* projMat;
    
    float mouseRelX;
    float mouseRelY;
    float screenWidth;
    float screenHeight;
    float lastMouseX;
    float lastMouseY;

    void registerEntity(Entity* entity);
    void registerCollider(Collider* collider);
    void unregisterCollider(Collider* collider);
    void clearDeadEntities();
    Vec3 getRandomPosition();
    void updateRelMousePos(const float dt);
    void updateKeyboardMovement(const float dt);
};

#endif // GAME_WORLD_H_INCLUDED
