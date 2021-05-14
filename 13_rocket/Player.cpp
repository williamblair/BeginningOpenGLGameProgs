#include <cassert>
#include <Player.h>
#include <SphereCollider.h>
#include <KeyboardInterface.h>
#include <GameWorld.h>

//#define DBG_ASSERT(cond) assert(cond)
#define DBG_ASSERT(cond)

Player::Player(GameWorld* const world) :
    Entity(world),
    position(Vec3()),
    velocity(Vec3()),
    yaw(0.0f),
    pitch(0.0f),
    collider(nullptr)
{
}

Player::~Player()
{
}

void Player::AddYaw(const float val)
{
    yaw += val;
    
    if (yaw >= 360.0f) yaw -= 360.0f;
    if (yaw < 0.0f) yaw += 360.0f;
}

void Player::AddPitch(const float val)
{
    pitch += val;
    
    const float PITCH_LIMIT = 45.0f;
    if (pitch >= PITCH_LIMIT) {
        pitch = PITCH_LIMIT;
    }
    
    if (pitch <= -PITCH_LIMIT) {
        pitch = -PITCH_LIMIT;
    }
}

void Player::MoveForward(const float speed)
{
    Vec3 pos = GetPosition();
    
    float cosYaw = cosf(deg2rad(yaw));
    float sinYaw = sinf(deg2rad(yaw));
    
    pos.x += cosYaw * speed;
    pos.z += sinYaw * speed;
    
    SetPosition(pos);
}

void Player::OnPrepare(float dt)
{
    KeyboardInterface* keys = GetWorld()->GetKeyboardInterface();
    MouseInterface* mouse = GetWorld()->GetMouseInterface();
    DBG_ASSERT(keys != nullptr);
    if (keys->IsKeyHeldDown(KeyboardCode::UP) ||
        keys->IsKeyHeldDown(KeyboardCode::W))
    {
        MoveForward(2.0f * dt);
    }
    if (keys->IsKeyHeldDown(KeyboardCode::DOWN) ||
        keys->IsKeyHeldDown(KeyboardCode::S))
    {
        MoveForward(-2.0f * dt);
    }
    if (keys->IsKeyHeldDown(KeyboardCode::LEFT) ||
        keys->IsKeyHeldDown(KeyboardCode::A))
    {
        AddYaw(-50.0f * dt);
    }
    if (keys->IsKeyHeldDown(KeyboardCode::RIGHT) ||
        keys->IsKeyHeldDown(KeyboardCode::D))
    {
        AddYaw(50.0f * dt);
    }

    // fire a rocket
    if (keys->IsKeyPressed(KeyboardCode::SPACE) ||
        mouse->IsButtonPressed(0))
    {
        Entity* rocket = GetWorld()->SpawnEntity(EntityType::ROCKET);
        if (rocket)
        {
            rocket->SetPosition(GetPosition());
            rocket->SetYaw(GetYaw());
            rocket->SetPitch(GetPitch());
        }
        else
        {
            std::cout << __func__ << ": player spawned NULL rocket" << std::endl;
        }
    }
    
    float x, y;
    GetWorld()->GetRelativeMousePos(x, y);
    AddYaw(x * 40.0f * dt);
    AddPitch(y * -40.0f * dt);
    
    position.y -= 8.0f * dt;
    
    // TODO - limit x/z pos based on terrain size
}

void Player::OnRender()
{
}

void Player::OnPostRender()
{
}

bool Player::OnInitialize()
{
    collider = std::unique_ptr<Collider>(
        new SphereCollider((Entity*)this, 0.75f));
    return true;
}

void Player::OnShutdown()
{
}

void Player::OnCollision(Entity* entity)
{
}

#undef DBG_ASSERT
