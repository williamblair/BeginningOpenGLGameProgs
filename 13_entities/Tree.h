#ifndef TREE_H_INCLUDED
#define TREE_H_INCLUDED

#include <string>
#include <vector>
#include <memory>

#include <GL/glew.h>

#include <Mat4.h>
#include <Vec3.h>
#include <Transform.h>
#include <Shader.h>
#include <TargaImage.h>
#include <Entity.h>
#include <EntityType.h>

// forward declarations
class GameWorld;

class Tree : public Entity
{
public:

    // Before drawing, the view and projection matrices for this
    // shader need to be set
    static Shader shader;

    Tree(GameWorld* world) :
        Entity(world)
    {}

    Tree(GameWorld* world,
         float x, float y, float z) :
        Entity(world)
    {
        transform.position = Vec3(x,y,z);
    }
    Tree(GameWorld* world,
         const Tree& t) :
        Entity(world)
    {
        transform = t.transform;
    }

    Tree& operator=(const Tree& t)
    {
        transform = t.transform;
        return *this;
    }

    EntityType GetType() const { return EntityType::TREE; }

    static bool Init();
    
    void Draw();

    virtual void OnPrepare(float dt);
    virtual void OnRender();
    virtual void OnPostRender();
    virtual bool OnInitialize();
    virtual void OnShutdown();

    // TODO
    virtual void OnCollision(Entity* collder) {}

    void SetPosition(const Vec3& v) { transform.position = v; }
    Vec3 GetPosition() const { return transform.position; }

    float GetYaw() const { return 0.0f; }
    float GetPitch() const { return 0.0f; }

    void SetYaw(const float val)   { (void)val; }
    void SetPitch(const float val) { (void)val; }

private:

    static GLuint texID;

    static GLuint vertexBufID;
    static GLuint texCoordBufID;

    Transform transform;
    Mat4 modelMat;

    static void InitBuffers();
};

#endif

