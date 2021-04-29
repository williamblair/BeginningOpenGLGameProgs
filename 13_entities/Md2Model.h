#ifndef MD2_MODEL_H_INCLUDED
#define MD2_MODEL_H_INCLUDED

#include <vector>
#include <string>
#include <cstdint>

#include <GL/glew.h>

#include <Vec3.h>
#include <Vec2.h>
#include <Shader.h>

class Md2Model
{
public:

    struct Animation
    {
        int32_t startFrame;
        int32_t endFrame;
        bool loop;
        
        Animation(int startFrame, int endFrame, bool loop = true) :
            startFrame(startFrame),
            endFrame(endFrame),
            loop(loop)
        {}

        static const Animation Idle;
        static const Animation Run;
        static const Animation Attack;
        static const Animation Pain1;
        static const Animation Pain2;
        static const Animation Pain3;
        static const Animation Jump;
        static const Animation FlipOff;
        static const Animation Salute;
        static const Animation Taunt;
        static const Animation Wave;
        static const Animation Point;
        static const Animation CrouchIdle;
        static const Animation CrouchWalk;
        static const Animation CrouchAttack;
        static const Animation CrouchPain;
        static const Animation CrouchDeath;
        static const Animation Death1;
        static const Animation Death2;
        static const Animation Death3;
    };

    Md2Model();
    ~Md2Model();
    
    bool Load(const std::string& fileName);
    void Update(const float dt);
    void Render(Shader& shader);
    
    void SetAnimation(const int start, const int end)
    {
        startFrame = start;
        endFrame = end;
        nextFrame = startFrame;
        currentAnimName = std::string(keyFrames[startFrame].name);
    }
    
    void SetAnimation(const Animation& anim)
    {
        SetAnimation(anim.startFrame, anim.endFrame);
        loopAnim = anim.loop;
    }
    
    inline std::string& GetAnimName()
    {
        return currentAnimName;
    }

private:

    struct Header
    {
        uint8_t magic[4];       // IDP2, 8
        int32_t version;
        
        int32_t skinWidth;      // tex width, height
        int32_t skinHeight;
        
        int32_t frameSize;      // size of 1 keyframe in bytes
        
        int32_t numSkins;       // number of textures
        int32_t numVertices;
        int32_t numTexCoords;
        int32_t numTriangles;
        int32_t numGLCmds;      // number of opengl draw commands
        int32_t numFrames;      // number of keyframes
        
        int32_t skinOffset;     // offset to skin names (64 bytes each)
        int32_t texCoordOffset; // offset to texture coords
        int32_t triangleOffset; // offset to triangles
        int32_t frameOffset;    // offset to keyframes
        int32_t GLCmdOffset;    // offset to opengl draw commands
        int32_t eofOffset;      // offset to end of file
    };
    
    struct Vertex
    {
        uint8_t v[3];   // compressed vertices; uncompress = scale*v + translate
        uint8_t lightNormIndex;
    };
    
    struct TexCoord
    {
        int16_t s;
        int16_t t;
    };
    
    struct KeyFrame
    {
        float scale[3];     // use to multiply and add Vertex::v
        float translate[3];
        char name[16];
        std::vector<Vertex> md2Vertices;
        std::vector<Vec3> vertices; // converted result vertices
    };
    
    struct Triangle
    {
        int16_t vertIndex[3];
        int16_t texCoordIndex[3];
    };
    
    struct Skin
    {
        char name[64]; // filename of the texture
    };

    std::vector<Skin> skins;
    std::vector<TexCoord> md2TexCoords;
    std::vector<Triangle> triangles;
    
    std::vector<Vec2> texCoords; // converted tex coords
    std::vector<KeyFrame> keyFrames;
    std::vector<std::string> texNames;
    
    KeyFrame interpolatedFrame; // tmp/worker frame
    std::string currentAnimName;
    
    int startFrame;
    int endFrame;
    int currentFrame;
    int nextFrame;
    float interpolation;
    bool loopAnim;
    
    GLuint vertexBufID;
    GLuint texCoordBufID;
    
    std::vector<float> radii; // radius for each frame
    
    void reorganizeVertices(); // recreate vertices so that we don't need to use indices
    void stripTextureNames(); // remove folder prefixes from texture names
    void genBuffers(); // create opengl buffers
};

#endif // MD2_MODEL_H_INCLUDED
