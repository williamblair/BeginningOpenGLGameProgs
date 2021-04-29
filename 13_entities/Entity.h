#ifndef ENTITY_H_INCLUDED
#define ENTITY_H_INCLUDED

/* Forward declarations */
class GameWorld;

class Entity
{
public:

    Entity(GameWorld* const gameWorld);
    virtual ~Entity();

    GameWorld* GetWorld() const {
        return gameWorld;
    }

    void Prepare(float dt);
    void Render();
    void PostRender();
    bool Init();
    void Shutdown();
    bool CanBeRemoved() const { return canBeRemoved; }
    void Destroy();
    void Collide(Entity* other);

    virtual Vec3 GetPosition() const = 0;
    virtual void SetPosition(const Vec3& pos) = 0;

private:

    virtual void OnPrepare(float dt) = 0;
    virtual void OnRender() = 0;
    virtual void OnPostRender() = 0;
    virtual bool OnInitialize() = 0;
    virtual void OnShutdown() = 0;
    virtual void OnCollision(Entity* collider) = 0;

    bool canBeRemoved;

    GameWorld* gameWorld;
};

#endif // ENTITY_H_INCLUDED
