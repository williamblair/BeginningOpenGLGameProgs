#include <vector>
#include <cstdlib>
#include <ctime>
#include <Window.h>
#include <Shader.h>
#include <HeightMap.h>
#include <Transform.h>
#include <TargaImage.h>
#include <Tree.h>

#ifdef __WIN32
#undef main
#endif

inline float deg2rad(const float deg) {
    return deg * M_PI / 180.0f;
}

inline Mat4 MakeModelMat(float angleDeg)
{
    Transform moveDown;
    Transform rotate;
    Transform moveBack;
    moveDown.position = Vec3(0.0f, -10.0f, 0.0f);
    rotate.rotation = angleAxis(deg2rad(angleDeg), Vec3(0.0f, 1.0f, 0.0f));
    moveBack.position = Vec3(0.0f, 0.0f, -30.0f);
    
    Mat4 downMat = transformToMat4(moveDown);
    Mat4 rotateMat = transformToMat4(rotate);
    Mat4 moveBackMat = transformToMat4(moveBack);
    return downMat *  moveBackMat * rotateMat;
}

inline GLuint MakeTexture(Targa::Image& tgaImage)
{
    GLuint texID;
    
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D,
                0, 
                GL_RGB8, 
                tgaImage.GetWidth(), 
                tgaImage.GetHeight(), 
                0,
                GL_RGB,
                GL_UNSIGNED_BYTE,
                tgaImage.GetImageData());

    return texID;
}

static void PositionTrees(const float heightThresh,
                          const float percent,
                          HeightMap& terrain,
                          std::vector<Tree>& trees)
{
    srand(time(0));
    for (int z = 0; z < 65; ++z)
    {
        for (int x = 0; x < 65; ++x)
        {
            Vec3 v = terrain.GetPositionAt(x, z);
            if (v.y > heightThresh)
            {
                if (rand()/(double(RAND_MAX) + 1) > percent)
                {
                    trees.push_back(Tree(v.x, v.y, v.z));
                }
            }
        }
    }
}

int main(int argc, char *argv[])
{
    Window window;
    Shader shader;
    Shader waterShader;
    HeightMap heightMap;
    Targa::Image grassTexture;
    Targa::Image waterTexture;
    std::vector<Tree> trees;

    if (!window.Initialize("Hello World", 640, 480)) {
        return 1;
    }
    if (!Tree::Init()) {
        return 1;
    }


    GLuint gVAO = 0;
    glGenVertexArrays(1, &gVAO);
    glBindVertexArray(gVAO);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);

    shader.Load("shaders/vertexShader.glsl", "shaders/fragmentShader.glsl");
    waterShader.Load("shaders/waterVertex.glsl", "shaders/waterFragment.glsl");
    shader.Bind();

    if (!heightMap.Load("assets/heightmap.raw")) {
        return 1;
    }
    if (!grassTexture.Load("assets/grass.tga")) {
        return 1;
    }
    if (!waterTexture.Load("assets/water.tga")) {
        return 1;
    }

    PositionTrees(5.0f, 0.94f, heightMap, trees);

    Mat4 projectionMatrix = perspective(52.0f,          // fov
                                        640.0/480.0,    // aspect
                                        1.0,            // near clip
                                        1000.0);         // far clip

    Mat4 viewMatrix; // = identity matrix

    float terrainAngle = 0.0f;
    Mat4 modelMatrix = MakeModelMat(terrainAngle);

    Mat4 normalMatrix = inverse(transposed(modelMatrix));

    Vec3 lightColor(1.0f,1.0f,1.0f);
    Vec3 lightPos(0.0f,18.0f,0.0f);
    Vec3 ambientColor(0.2f,0.2f,0.2f);


    float lightPosZ = 0.0f;
    float lightDir = 1.0f;
    const float LIGHT_SPEED = 0.04f;
#define UpdateLightPos()        \
    if (lightPosZ > 20.0f) {    \
        lightDir = -1.0f;       \
    }                           \
    if (lightPosZ < -40.0f) {   \
        lightDir = 1.0f;        \
    }                           \
    lightPosZ += lightDir * LIGHT_SPEED * dt;   \
    lightPos.z = lightPosZ

#define UpdateTerrainUniforms() \
    shader.SetUniform("uModel", modelMatrix);           \
    shader.SetUniform("uView", viewMatrix);             \
    shader.SetUniform("uProjection", projectionMatrix); \
    shader.SetUniform("uNormal", normalMatrix);         \
    shader.SetUniform("uTexture0", 0);                  \
    shader.SetUniform("uLightColor", lightColor);       \
    shader.SetUniform("uLightPos", lightPos);           \
    shader.SetUniform("uAmbientColor", ambientColor)

#define UpdateWaterUniforms()                                   \
    waterShader.SetUniform("uModel", modelMatrix);              \
    waterShader.SetUniform("uView", viewMatrix);                \
    waterShader.SetUniform("uProjection", projectionMatrix);    \
    waterShader.SetUniform("uTexture0", 0)                   

    Mat4 treeView = viewMatrix * modelMatrix;
#define UpdateTreeUniforms()    \
    treeView = viewMatrix * modelMatrix; \
    Tree::shader.SetUniform("uView", treeView);  \
    Tree::shader.SetUniform("uProjection", projectionMatrix);    \
    Tree::shader.SetUniform("uTexture0", 0)                   

    GLuint grassTexID = MakeTexture(grassTexture);
    GLuint waterTexID = MakeTexture(waterTexture);

    Uint32 lastTicks = SDL_GetTicks();
    do
    {
        glClearColor(0.1, 0.2, 0.3, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.Bind();
        UpdateTerrainUniforms();
        glBindTexture(GL_TEXTURE_2D, grassTexID);
        heightMap.Draw(shader);

        waterShader.Bind();
        UpdateWaterUniforms();
        glBindTexture(GL_TEXTURE_2D, waterTexID);
        heightMap.DrawWater(waterShader);

        Tree::shader.Bind();
        UpdateTreeUniforms();
        for (Tree& tree : trees)
        {
            tree.Draw();
        }
        
        Uint32 thisTicks = SDL_GetTicks();
        float dt = thisTicks - lastTicks;
        lastTicks = thisTicks;

        terrainAngle += 0.01f * dt;
        if (terrainAngle >= 360.0f) {
            terrainAngle -= 360.0f;
        }
        modelMatrix = MakeModelMat(terrainAngle);

        UpdateLightPos();
        
        window.Update(dt);

    } while (!window.ShouldQuit());

    return 0;
}

