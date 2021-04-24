#ifndef TREE_H_INCLUDED
#define TREE_H_INCLUDED

#include <string>
#include <vector>
#include <memory>

#include <GL/glew.h>

#include <Mat4.h>
#include <Transform.h>
#include <Shader.h>
#include <TargaImage.h>

class Tree
{
public:

    // Before drawing, the view and projection matrices for this
    // shader need to be set
    static Shader shader;

    Tree() :
        x(0.0f),
        y(0.0f),
        z(0.0f)
    {}

    Tree(float x, float y, float z) :
        x(x),
        y(y),
        z(z)
    {}
    Tree(const Tree& t) :
        x(t.x),
        y(t.y),
        z(t.z)
    {}

    Tree& operator=(const Tree& t)
    {
        x = t.x;
        y = t.y;
        z = t.z;

        return *this;
    }

    static bool Init();
    
    void Draw();

private:

    float x;
    float y;
    float z;

    static GLuint texID;

    static GLuint vertexBufID;
    static GLuint texCoordBufID;

    Transform transform;
    Mat4 modelMat;

    static void InitBuffers();
};

#endif

