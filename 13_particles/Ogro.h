#ifndef OGRO_H_INCLUDED
#define OGRO_H_INCLUDED

#include <string>

#include <GL/glew.h>

#include <Transform.h>
#include <Md2Model.h>
#include <TargaImage.h>
#include <Shader.h>

static inline float deg2rad(const float deg) {
    return deg * M_PI / 180.0f;
}

class Ogro
{
public:
    Ogro(){}
    ~Ogro(){}
    
    bool Init()
    {
        if (!model.Load("assets/models/Ogro/tris.md2")) {
            return false;
        }
        if (!texture.Load("assets/models/Ogro/Ogrobase.tga")) {
            return false;
        }
        
        texID = Targa::Image::MakeTexture(texture);
        
        transform.rotation = angleAxis(deg2rad(-90.0f), Vec3(0.0f, 1.0f, 0.0f));
        transform.position.z = -2.0f;
        
        return true;
    }
    
    void Update(float dt)
    {
        model.Update(dt);
    }
    
    void Render(Shader& shader)
    {
        Mat4 modelMat = transformToMat4(transform);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texID);
        shader.SetUniform("uTexture0", 0);
        shader.SetUniform("uModel", modelMat);
        model.Render(shader);
    }
    
    inline std::string& GetAnimName() {
        return model.GetAnimName();
    }
    
    inline void SetAnim(const Md2Model::Animation* anim) {
        model.SetAnimation(*anim);
    }
    
private:
    Md2Model model;
    Targa::Image texture;
    GLuint texID;
    Transform transform;
};

#endif // OGRO_H_INCLUDED

