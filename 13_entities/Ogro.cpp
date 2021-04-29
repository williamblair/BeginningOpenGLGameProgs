#include <Ogro.h>

Ogro::Ogro(GameWorld* world) :
    Enemy(world),
    aiState(AIState::IDLE),
    currentTime(0),
    lastAIChange(0)
{

}

Ogro::~Ogro()
{

}

void Ogro::OnPrepare(float dt)
{}
void Ogro::OnRender()
{}
void Ogro::OnPostRender()
{}
bool Ogro::OnInitialize()
{
    return true;
}
void Ogro::OnShutdown()
{}
void Ogro::OnKill()
{}
void Ogro::OnResurrection()
{}
