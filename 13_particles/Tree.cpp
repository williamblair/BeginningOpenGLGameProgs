#include <Tree.h>

Shader Tree::shader;
GLuint Tree::texID = 0;

GLuint Tree::vertexBufID = 0;
GLuint Tree::texCoordBufID = 0;

const std::string TreeTextureImage = "assets/beech.tga"; 
const std::string TreeVertexShader = "shaders/alphaTestVertex.glsl";
const std::string TreeFragmentShader = "shaders/alphaTestFragment.glsl";

bool Tree::Init()
{
    // only load tree texture once
    if (texID == 0)
    {
        Targa::Image treeTexture;
        if (!treeTexture.Load(TreeTextureImage)) {
            return false;
        }

        glGenTextures(1, &texID);
        glBindTexture(GL_TEXTURE_2D, texID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D,
                    0, 
                    GL_RGBA8, 
                    treeTexture.GetWidth(), 
                    treeTexture.GetHeight(), 
                    0,
                    GL_RGBA,
                    GL_UNSIGNED_BYTE,
                    treeTexture.GetImageData());

        shader.Load(TreeVertexShader, TreeFragmentShader);

        InitBuffers();
    }

    return true;
}

void Tree::Draw()
{
    transform.position.x = x;
    transform.position.y = y;
    transform.position.z = z;
    modelMat = transformToMat4(transform);

    shader.Bind();
    GLuint aPosition = shader.GetAttribute("aPosition");
    GLuint aTexCoord0 = shader.GetAttribute("aTexCoord0");
    shader.SetUniform("uModel", modelMat);

    glBindTexture(GL_TEXTURE_2D, texID);

    glDisable(GL_CULL_FACE);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufID);
    glVertexAttribPointer(aPosition,
                          3, // Vec3 x,y,z
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          0);
    glEnableVertexAttribArray(aPosition);

    glBindBuffer(GL_ARRAY_BUFFER, texCoordBufID);
    glVertexAttribPointer(aTexCoord0,
                          2, // Vec2 x,y
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          0);
    glEnableVertexAttribArray(aTexCoord0);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);

    glDisableVertexAttribArray(aPosition);
    glDisableVertexAttribArray(aTexCoord0);
    
    glEnable(GL_CULL_FACE);
}

void Tree::InitBuffers()
{
    GLfloat vertices[] = 
    {
        -1.0f, 0.0f, 0.0f, // first square
        1.0f, 0.0f, 0.0f,
        -1.0f, 2.0f, 0.0f,
        1.0f, 2.0f, 0.0f,

        0.0f, 0.0f, 1.0f, // second square
        0.0f, 0.0f, -1.0f,
        0.0f, 2.0f, 1.0f,
        0.0f, 2.0f, -1.0f
    };

    GLfloat texCoords[] =
    {
        0.0f, 0.0f, // first square
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,

        0.0f, 0.0f, // second square
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f
    };

    glGenBuffers(1, &vertexBufID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufID);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(GLfloat) * 8 * 3,
                 (void*)vertices,
                 GL_STATIC_DRAW);

    glGenBuffers(1, &texCoordBufID);
    glBindBuffer(GL_ARRAY_BUFFER, texCoordBufID);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(GLfloat) * 8 * 2,
                 (void*)texCoords,
                 GL_STATIC_DRAW);
}

