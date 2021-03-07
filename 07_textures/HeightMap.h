#ifndef HEIGHT_MAP_H_INCLUDED
#define HEIGHT_MAP_H_INCLUDED

#include <GL/glew.h>

#include <vector>
#include <string>
#include <cstdint>
#include <Vec2.h>
#include <Vec3.h>
#include <Shader.h>

class HeightMap
{
public:
    HeightMap();
    virtual ~HeightMap();

    bool Load(const std::string& rawFileName,
              const unsigned int width = 65,
              const float heightScale = 10.0f,
              const float sizeScale = 1.0f);

    void Draw(Shader& shader);

private:

    std::vector<Vec3> positions;
    std::vector<float> heights;
    std::vector<Vec3> colors;
    std::vector<Vec2> texCoords;
    std::vector<uint32_t> indices;
    unsigned int width;

    GLuint vertexBuffer;
    GLuint colorBuffer;
    GLuint texCoordBuffer;
    GLuint indexBuffer;

    void GenerateVertices(const float sizeScale);
    void GenerateTexCoords();
    void GenerateIndices();
    void GenerateBuffers();
};

#endif // HEIGHT_MAP_H_INCLUDED

