#include <Ogro.h>
#include <Collider.h>
#include <HeightMap.h>
#include <iostream>

#define OGRO_MODEL "assets/models/Ogro/tris.md2"
#define OGRO_TEXTURE "assets/models/Ogro/Ogrobase.tga"

#define OGRO_VERT_SHADER "shaders/ogroVert.glsl"
#define OGRO_FRAG_SHADER "shaders/ogroFrag.glsl"

bool   Ogro::shaderLoaded = false;
Shader Ogro::shader;

Ogro::Ogro(GameWorld* world) :
    Enemy(world),
    aiState(AIState::IDLE),
    currentTime(0),
    lastAIChange(0)
{
    model = new Md2Model();
}

Ogro::~Ogro()
{
    delete model;
}

void Ogro::OnPrepare(float dt)
{
    GetCollider()->SetRadius(model->GetRadius());

    currentTime += dt;
    processAI();

    model->Update(dt);

    // try and force against the ground
    if (position.y > 0.0f) {
         position.y -= 10.0f * dt;
    }
    
    Vec3 pos = GetPosition();

    float speed = 0.0f;

    if (aiState == AIState::RUNNING)
    {
        speed = 2.0f * dt;
    }
    else if (aiState == AIState::WALK)
    {
        speed = 0.5f * dt;
    }

    float cosYaw = cosf(deg2rad(yaw));
    float sinYaw = sinf(deg2rad(yaw));
    pos.x += cosYaw * speed;
    pos.z += sinYaw * speed;
    SetPosition(pos);
}

void Ogro::OnRender()
{
    Transform tf;
    shader.Bind();
    tf.position = GetPosition();
    tf.rotation = angleAxis(deg2rad(yaw), Vec3(0.0f, -1.0f, 0.0f));
    Mat4 modelMat = transformToMat4(tf);
    Mat4* viewMat = gameWorld->GetViewMatrix();
    Mat4* projMat = gameWorld->GetProjectionMatrix();
    shader.SetUniform("uModel", modelMat);
    shader.SetUniform("uView", *viewMat);
    shader.SetUniform("uProjection", *projMat);
    glBindTexture(GL_TEXTURE_2D, texID);
    shader.SetUniform("uTexture0", 0);
    model->Render(shader);
}

void Ogro::OnPostRender()
{
}

bool Ogro::OnInitialize()
{
    if (!shaderLoaded)
    {
        shader.Load(OGRO_VERT_SHADER, OGRO_FRAG_SHADER);
        shaderLoaded = true;
    }
    if (!model->Load(OGRO_MODEL)) {
        return false;
    }
    if (!texture.Load(OGRO_TEXTURE)) {
        return false;
    }
    model->SetAnimation(Md2Model::Animation::Idle);
    texID = Targa::Image::MakeTexture(texture);
    SetYaw((float(rand()) / RAND_MAX) * 360.0f);
    return true;
}

void Ogro::OnShutdown()
{}

void Ogro::OnKill()
{
    int r = rand() % 3;
    switch (r)
    {
    case 0:  model->SetAnimation(Md2Model::Animation::Death1); break;
    case 1:  model->SetAnimation(Md2Model::Animation::Death2); break;
    default: model->SetAnimation(Md2Model::Animation::Death3); break;
    }

    aiState = AIState::DEAD;
}

void Ogro::OnResurrection()
{
    model->SetAnimation(Md2Model::Animation::Idle);
    aiState = AIState::IDLE;
    lastAIChange = currentTime;
}

static Ogro::AIState getRandomIdleState()
{
    int result = rand() % 3;
    switch (result)
    {
    case 0: return Ogro::AIState::IDLE;
    case 1: return Ogro::AIState::CROUCH;
    case 2: return Ogro::AIState::WALK;
    default: return Ogro::AIState::IDLE;
    }

    // should never occur
    return Ogro::AIState::IDLE;
}

void Ogro::processAI()
{
    if (IsDead()) {
        return;
    }

    const float DANGER_DISTANCE = 5.0f;

    // TODO player distance
    const float playerDistance = 10.0f;

    // Randomly move
    if (playerDistance > DANGER_DISTANCE)
    {
        if (((currentTime + float(rand() % 5) / 10.0f) - lastAIChange) > 8.0f)
        {
            AIState newState = getRandomIdleState();
            if (newState != aiState)
            {
                aiState = newState;
                lastAIChange = currentTime;
                if (newState == AIState::IDLE)
                {
                    model->SetAnimation(Md2Model::Animation::Idle);
                }
                else if (newState == AIState::CROUCH)
                {
                    model->SetAnimation(Md2Model::Animation::CrouchIdle);
                }
                else if (newState == AIState::WALK)
                {
                    model->SetAnimation(Md2Model::Animation::CrouchWalk);
                    yaw += float(rand() % 180) - 90.0f;
                }
            }
        }
    }

    // Prevent moving outside of map bounds
    float minX = GetWorld()->GetHeightMap()->GetMinX() + 2.5f;
    float minZ = GetWorld()->GetHeightMap()->GetMinZ() + 2.5f;
    float maxX = GetWorld()->GetHeightMap()->GetMaxX() - 2.5f;
    float maxZ = GetWorld()->GetHeightMap()->GetMaxZ() - 2.5f;

    // random other direction to turn if we hit the edge
    float randYaw = 90.0f + (float)(rand() % 90);

    Vec3 pos = GetPosition();
    if (pos.x < minX ||
        pos.x > maxX ||
        pos.z < minZ ||
        pos.z > maxZ)
    {
        yaw += randYaw;
        aiState = AIState::WALK;
        model->SetAnimation(Md2Model::Animation::Run);
        lastAIChange = currentTime;

        if (pos.x < minX) pos.x = minX;
        if (pos.x > maxX) pos.x = maxX;
        if (pos.z < minZ) pos.z = minZ;
        if (pos.z > maxZ) pos.z = minZ;
    }
}

