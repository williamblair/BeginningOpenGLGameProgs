#include <Ogro.h>
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
    // TODO - collider

    currentTime += dt;
    processAI();

    model->Update(dt);

    // TODO - try and force against the ground
    // if (position.y > 0.0f) {
    //      position.y -= 10.0f * dt;
    // }
    
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

    // TODO
    // pos.x += cosYaw * speed;
    // pos.z += sinYaw * speed;
    // SetPosition(pos);
    
    // debug test
    yaw += 20.0f * dt;
    if (yaw >= 360.0f) {
        yaw -= 360.0f;
    }
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

void Ogro::processAI()
{
    if (IsDead()) {
        return;
    }

    const float DANGER_DISTANCE = 5.0f;

    // TODO player distance
    // TODO - randomly move
}

