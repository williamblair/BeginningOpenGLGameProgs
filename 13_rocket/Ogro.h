#ifndef OGRO_H_INCLUDED
#define OGRO_H_INCLUDED

#include <string>

#include <GL/glew.h>

#include <Transform.h>
#include <Md2Model.h>
#include <TargaImage.h>
#include <Shader.h>
#include <Enemy.h>
#include <EntityType.h>
#include <GameWorld.h>

class Ogro : public Enemy
{
public:

    enum class AIState
    {
        IDLE,
        RUNNING,
        CROUCH,
        WALK,
        DEAD
    };

    Ogro(GameWorld* world);
    virtual ~Ogro();

    virtual EntityType GetType() const { return EntityType::OGRO; }

private:

    Md2Model* model;
    Targa::Image texture;
    GLuint texID;
    static bool shaderLoaded;
    static Shader shader;

    AIState aiState;

    float currentTime;
    float lastAIChange;

    void processAI();

    virtual void OnPrepare(float dt);
    virtual void OnRender();
    virtual void OnPostRender();
    virtual bool OnInitialize();
    virtual void OnShutdown();
    virtual void OnKill();
    virtual void OnResurrection();

    inline float deg2rad(const float deg)
    {
        return deg * 3.1415926535 / 180.0f;
    }
};

#endif // OGRO_H_INCLUDED
