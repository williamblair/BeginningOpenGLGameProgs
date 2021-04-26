#ifndef EXPLOSION_H_INCLUDED
#define EXPLOSION_H_INCLUDED

#include <vector>
#include <memory>

#include <GL/glew.h>

#include <Vec3.h>
#include <Vec4.h>
#include <TargaImage.h>
#include <Shader.h>
#include <Transform.h>

class Explosion
{
public:

    struct Particle
    {
        float life; // how long the particle exists
        Vec3 position;
        Vec3 velocity;
        Vec4 color;
    };

    Explosion(unsigned int numParticles);
    ~Explosion();

    bool Init();
    void Update(float dt);
    void Render();

    Vec3 GetPosition() const 
    {
        return transform.position;
    }
    void SetPosition(Vec3& v)
    {
        transform.position = v;
        for (Particle& p : particles)
        {
            p.position = v;
        }
    }
    void SetPosition(float x, float y, float z)
    {
        Vec3 newPos(x, y, z);
        SetPosition(newPos);
    }

    Shader& GetShader()
    {
        return shader;
    }


private:
    
    std::vector<Particle> particles;

    static GLuint vertBufID;
    static GLuint colorBufID;

    static Targa::Image particleTexture;
    static GLuint texID;

    static Shader shader;

    Transform transform;

    bool initTexture();
    bool initBuffers();
};

#endif // EXPLOSION_H_INCLUDED

