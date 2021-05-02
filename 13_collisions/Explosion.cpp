#include <Explosion.h>
#include <iostream>

#define PARTICLE_TEXTURE "assets/particle.tga"
#define VERT_SHADER "shaders/particleVert.glsl"
#define FRAG_SHADER "shaders/particleFrag.glsl"

GLuint       Explosion::vertBufID = 0;
GLuint       Explosion::colorBufID = 0;
Targa::Image Explosion::particleTexture;
GLuint       Explosion::texID;
Shader       Explosion::shader;

Explosion::Explosion(unsigned int numParticles) :
    particles(numParticles)
{
}

Explosion::~Explosion()
{
}

bool Explosion::Init()
{
    std::vector<Vec4> colors = {
        Vec4(1.0f, 0.0f, 0.0f, 1.0f),
        Vec4(1.0f, 1.0f, 0.0f, 1.0f),
        Vec4(1.0f, 0.5f, 0.0f, 1.0f),
        Vec4(0.5f, 0.5f, 0.5f, 1.0f)
    };

    for (Particle& p : particles)
    {
        float rndVx = rand() / ((float)RAND_MAX + 1) - 0.5f;
        float rndVy = rand() / ((float)RAND_MAX + 1) - 0.5f;
        float rndVz = rand() / ((float)RAND_MAX + 1) - 0.5f;
        float speed = 1.5f;

        p.life = 1.0f;
        p.velocity = Vec3(rndVx, rndVy, rndVz);
        normalize(p.velocity);
        p.velocity = p.velocity * speed;

        p.position = GetPosition();

        int rndColor = rand() % colors.size();
        p.color = colors[rndColor];
    }

    if (texID == 0)
    {
        if (!initTexture()) {
            return false;
        }
        shader.Load(VERT_SHADER, FRAG_SHADER);
        shader.Bind();
        if (!initBuffers()) {
            return false;
        }
        shader.UnBind();
    }

    return true;
}

bool Explosion::initTexture()
{
    if (!particleTexture.Load(PARTICLE_TEXTURE)) {
        return false;
    }

    glGenTextures(1, &texID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGB,
                 particleTexture.GetWidth(), particleTexture.GetHeight(),
                 0,
                 GL_RGB,
                 GL_UNSIGNED_BYTE,
                 particleTexture.GetImageData());

    return true;
}

bool Explosion::initBuffers()
{
    glGenBuffers(1, &vertBufID);
    glBindBuffer(GL_ARRAY_BUFFER, vertBufID);
    glBufferData(GL_ARRAY_BUFFER,
                 particles.size() * sizeof(Vec3),
                 NULL,
                 GL_DYNAMIC_DRAW);

    glGenBuffers(1, &colorBufID);
    glBindBuffer(GL_ARRAY_BUFFER, colorBufID);
    glBufferData(GL_ARRAY_BUFFER,
                 particles.size() * sizeof(Vec4),
                 NULL,
                 GL_DYNAMIC_DRAW);

    return true;
}

void Explosion::Update(float dt)
{
    int numDead = 0;
    for (Particle& p : particles)
    {
        if (p.life <= 0.0f)
        {
            ++numDead;
            continue;
        }

        p.life -= 1.0f * dt;
        p.position = (p.position + (p.velocity * dt));
    }

    // TODO
    //if (numDead == particles.size())
    //{
    //    destroy();
    //}
}

void Explosion::Render()
{
    // The collection of not-dead particles to render
    static std::vector<Vec3> positions;
    static std::vector<Vec4> colors;
    
    Mat4 modelMatrix = transformToMat4(transform);

    shader.SetUniform("uModel", modelMatrix);
    shader.SetUniform("uPointSize", 10.0f);
    shader.SetUniform("uTexture0", 0);

    positions.clear();
    colors.clear();

    glBindTexture(GL_TEXTURE_2D, texID);
    for (Particle& p : particles)
    {
        if (p.life <= 0.0f)
        {
            continue;
        }

        positions.push_back(p.position);
        Vec4 color = p.color;
        color.w = p.life;
        colors.push_back(color);
    }

    //std::cout << "Particles size: " << positions.size() << std::endl;
    //for (Vec3& v : positions)
    //{
    //    std::cout << "  " << v.x << ", " << v.y << ", " << v.z << std::endl;
    //}

    // no need to render if all particles are dead
    if (positions.empty())
    {
        return;
    }

    glBindBuffer(GL_ARRAY_BUFFER, vertBufID);
    glBufferSubData(GL_ARRAY_BUFFER,
                    0,
                    positions.size() * sizeof(Vec3),
                    positions.data());

    glBindBuffer(GL_ARRAY_BUFFER, colorBufID);
    glBufferSubData(GL_ARRAY_BUFFER,
                    0,
                    colors.size() * sizeof(Vec4),
                    colors.data());

    GLuint aPosition = shader.GetAttribute("aPosition");
    GLuint aColor = shader.GetAttribute("aColor");

    glBindBuffer(GL_ARRAY_BUFFER, vertBufID);
    glVertexAttribPointer(aPosition,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          0);
    glEnableVertexAttribArray(aPosition);

    glBindBuffer(GL_ARRAY_BUFFER, colorBufID);
    glVertexAttribPointer(aColor,
                          4,
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          0);
    glEnableVertexAttribArray(aColor);

    //glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glEnable(GL_PROGRAM_POINT_SIZE);
    glDrawArrays(GL_POINTS, 0, positions.size());
    glDisable(GL_PROGRAM_POINT_SIZE);

    glDisable(GL_BLEND);
    //glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);

    glDisableVertexAttribArray(aPosition);
    glDisableVertexAttribArray(aColor);
}

