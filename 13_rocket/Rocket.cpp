#include <Rocket.h>
#include <Md2Model.h>
#include <SphereCollider.h>
#include <Transform.h>
#include <GameWorld.h>

#define ROCKET_MODEL "assets/models/Rocket/rocket.md2"
#define ROCKET_TEXTURE "assets/models/Rocket/rocket.tga"
#define ROCKET_VERTEX_SHADER "shaders/ogroVert.glsl" // same shader as other md2 model
#define ROCKET_FRAGMENT_SHADER "shaders/ogroFrag.glsl"

bool Rocket::shaderLoaded = false;
Shader Rocket::shader;

Rocket::Rocket(GameWorld* const gameWorld) :
    Entity(gameWorld),
    position(0.0f, 0.0f, 0.0f),
    yaw(0.0f),
    pitch(0.0f),
    collider(nullptr),
    model(nullptr),
    rocketTexId(0),
    rocketTexture(Targa::Image())
{
}

Rocket::~Rocket()
{
    if (collider != nullptr)
    {
        delete collider;
    }
    if (model != nullptr)
    {
        delete model;
    }
}

void Rocket::OnPrepare(float dt)
{
    GetCollider()->SetRadius(model->GetRadius());
    
    Vec3 velocity;
    
    float yawRad = deg2rad(yaw);
    float pitchRad = deg2rad(pitch);
    float cosYaw = cosf(yawRad);
    float sinYaw = sinf(yawRad);
    float cosPitch = cosf(pitchRad);
    float sinPitch = sinf(pitchRad);
    
    const float speed = 20.0f;
    
    velocity.x = cosPitch * cosYaw * speed;
    velocity.y = sinPitch * speed;
    velocity.z = cosPitch * sinYaw * speed;
    
    //const Vec3 gravity(0.0f, -1.0f, 0.0f);
    
    Vec3 curPos = GetPosition();
    curPos = curPos + (velocity * dt);
    SetPosition(curPos);
}

void Rocket::OnRender()
{
    Transform tf;
    Transform tfRotate2;
    shader.Bind();
    tf.position = GetPosition();
    tf.scale = Vec3(0.5f, 0.5f, 0.5f);
    tf.rotation = angleAxis(deg2rad(yaw), Vec3(0.0f, -1.0f, 0.0f));
    tfRotate2.rotation = angleAxis(deg2rad(pitch), Vec3(0.0f, 0.0f, 1.0f));
    Mat4 modelMat = transformToMat4(tf);
    Mat4 modelMatRotate2 = transformToMat4(tfRotate2);
    modelMat = modelMatRotate2 * modelMat;
    Mat4* viewMat = GetWorld()->GetViewMatrix();
    Mat4* projMat = GetWorld()->GetProjectionMatrix();
    shader.SetUniform("uModel", modelMat);
    shader.SetUniform("uView", *viewMat);
    shader.SetUniform("uProjection", *projMat);
    glBindTexture(GL_TEXTURE_2D, rocketTexId);
    shader.SetUniform("uTexture0", 0);
    model->Render(shader);
}
void Rocket::OnPostRender()
{
}

bool Rocket::OnInitialize()
{
    if (!shaderLoaded) {
        shader.Load(ROCKET_VERTEX_SHADER, ROCKET_FRAGMENT_SHADER);
        shaderLoaded = true;
    }
    
    collider = new SphereCollider((Entity*)this, 0.0f);
    if (!collider)
    {
        std::cout << __func__ << ": failed to create rocket sphere collider"
                  << std::endl;
        return false;
    }
    model = new Md2Model();
    if (!model)
    {
        std::cout << __func__ << ": failed to create rocket md2model"
                  << std::endl;
        return false;
    }
    if (!model->Load(ROCKET_MODEL))
    {
        std::cout << __func__ << ": failed to load rocket md2model"
                  << std::endl;
        return false;
    }
    if (!rocketTexture.Load(ROCKET_TEXTURE))
    {
        std::cout << __func__ << ": failed to load rocket texture"
                  << std::endl;
        return false;
    }
    rocketTexId = Targa::Image::MakeTexture(rocketTexture);
    
    return true;
}

void Rocket::OnShutdown()
{
}

void Rocket::OnCollision(Entity* other)
{
    if (other->GetType() == EntityType::PLAYER) {
        return;
    }
    // TODO - create explosion
    std::cout << "Rocket oncollision - create explosion!" << std::endl;
    
    Destroy(); // mark the rocket as dead
}

#undef ROCKET_MODEL
#undef ROCKET_TEXTURE
