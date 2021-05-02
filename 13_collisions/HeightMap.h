#ifndef HEIGHT_MAP_H_INCLUDED
#define HEIGHT_MAP_H_INCLUDED

#include <GL/glew.h>

#include <vector>
#include <string>
#include <cstdint>
#include <cassert>
#include <memory>
#include <Vec2.h>
#include <Vec3.h>
#include <Transform.h>
#include <Shader.h>
#include <Entity.h>
#include <TargaImage.h>
#include <GameWorld.h>

#define DBGASSERT(x) assert(x)
//#define DBGASSERT(x)

class HeightMap : public Entity
{
public:
    HeightMap(GameWorld* const gameWorld);
    virtual ~HeightMap();

    virtual EntityType GetType() const { return EntityType::LANDSCAPE; }

    bool Load(const std::string& rawFileName,
              const unsigned int width = 65,
              const float heightScale = 10.0f,
              const float sizeScale = 1.0f);

    void Draw(Shader& shader);
    void DrawWater(Shader& shader);

    inline Vec3 GetPositionAt(int x, int z)
    {
        DBGASSERT((z * width) + x < (int)positions.size());
        return positions[(z * width) + x];
    }

    Vec3 GetPosition() const { return transform.position; }
    void SetPosition(const Vec3& pos) { transform.position = pos; }

    inline float GetMinX() const { return minX; }
    inline float GetMinZ() const { return minZ; }
    inline float GetMaxX() const { return maxX; }
    inline float GetMaxZ() const { return maxZ; }

    float GetHeightAt(const float x, const float z) const;

    virtual Collider* GetCollider() { return collider.get(); }

private:

    std::vector<Vec3> positions;
    std::vector<float> heights;
    std::vector<Vec3> colors;
    std::vector<Vec3> normals;
    std::vector<Vec2> texCoords;
    std::vector<uint32_t> indices;
    unsigned int width;
    float minX;
    float minZ;
    float maxX;
    float maxZ;
    std::shared_ptr<Collider> collider;

    std::vector<Vec3> waterPositions;
    std::vector<uint32_t> waterIndices;
    std::vector<Vec2> waterTexCoords;

    GLuint vertexBuffer;
    GLuint colorBuffer;
    GLuint normalBuffer;
    GLuint texCoordBuffer;
    GLuint indexBuffer;

    GLuint waterVertexBuffer;
    GLuint waterIndexBuffer;
    GLuint waterTexCoordBuffer;

    static bool shaderLoaded;
    static Targa::Image grassTexture;
    static GLuint grassTexID;
    static Shader heightmapShader;

    Transform transform;

    void GenerateVertices(const float sizeScale);
    void GenerateTexCoords();
    void GenerateNormals();
    void GenerateIndices();
    void GenerateBuffers();

    void GenerateWaterVertices();
    void GenerateWaterTexCoords();
    void GenerateWaterIndices();

    virtual void OnPrepare(float dt);
    virtual void OnRender();
    virtual void OnPostRender();
    virtual bool OnInitialize();
    virtual void OnShutdown();
    virtual void OnCollision(Entity* entity);
};

#endif // HEIGHT_MAP_H_INCLUDED

