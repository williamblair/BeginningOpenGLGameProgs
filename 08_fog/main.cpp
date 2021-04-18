#include <Window.h>
#include <Shader.h>
#include <HeightMap.h>
#include <Transform.h>
#include <TargaImage.h>

#ifdef __WIN32
#undef main
#endif

enum class FogMode : unsigned int
{
    Linear = 0,
    Exp = 1,
    ExpSquared = 2
};

inline float deg2rad(const float deg) {
    return deg * M_PI / 180.0f;
}

inline Mat4 MakeModelMat(float angleDeg)
{
    Transform moveDown;
    Transform rotate;
    Transform moveBack;
    moveDown.position = Vec3(0.0f, -15.0f, 0.0f);
    rotate.rotation = angleAxis(deg2rad(angleDeg), Vec3(0.0f, 1.0f, 0.0f));
    moveBack.position = Vec3(0.0f, 0.0f, -50.0f);
    
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

int main(int argc, char *argv[])
{
    Window window;
    Shader shader;
    Shader waterShader;
    HeightMap heightMap;
    Targa::Image grassTexture;
    Targa::Image waterTexture;
    FogMode fogMode = FogMode::Linear;

    if (!window.Initialize("Hello World", 640, 480)) {
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

    Vec4 fogColor(0.5f, 0.5f, 0.5f, 0.5f);
    float fogStart = 20.0f;
    float fogEnd = 50.0f;
    float fogDensity = 0.03f;

#define UpdateTerrainUniforms() \
    shader.SetUniform("uModel", modelMatrix);           \
    shader.SetUniform("uView", viewMatrix);             \
    shader.SetUniform("uProjection", projectionMatrix); \
    shader.SetUniform("uNormal", normalMatrix);         \
    shader.SetUniform("uTexture0", 0);                  \
    shader.SetUniform("uLightColor", lightColor);       \
    shader.SetUniform("uLightPos", lightPos);           \
    shader.SetUniform("uAmbientColor", ambientColor);   \
    shader.SetUniform("uFogColor", fogColor);           \
    shader.SetUniform("uFogStart", fogStart);           \
    shader.SetUniform("uFogEnd", fogEnd);               \
    shader.SetUniform("uFogDensity", fogDensity);       \
    shader.SetUniform("uFogType", (int)fogMode)

#define UpdateWaterUniforms()                                   \
    waterShader.SetUniform("uModel", modelMatrix);              \
    waterShader.SetUniform("uView", viewMatrix);                \
    waterShader.SetUniform("uProjection", projectionMatrix);    \
    waterShader.SetUniform("uTexture0", 0);                     \
    waterShader.SetUniform("uFogColor", fogColor);              \
    waterShader.SetUniform("uFogStart", fogStart);              \
    waterShader.SetUniform("uFogEnd", fogEnd);                  \
    waterShader.SetUniform("uFogDensity", fogDensity);          \
    waterShader.SetUniform("uFogType", (int)fogMode)

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
        
        Uint32 thisTicks = SDL_GetTicks();
        float dt = thisTicks - lastTicks;
        lastTicks = thisTicks;

        terrainAngle += 0.01f * dt;
        if (terrainAngle >= 360.0f) {
            terrainAngle -= 360.0f;
        }
        modelMatrix = MakeModelMat(terrainAngle);
        normalMatrix = inverse(transposed(modelMatrix));

        UpdateLightPos();

        if (window.SpacebarReleased()) {
            std::string newMode;
            switch (fogMode)
            {
            case FogMode::Linear:
                fogMode = FogMode::Exp;
                newMode = "Exp";
                break;
            case FogMode::Exp:
                fogMode = FogMode::ExpSquared;
                newMode = "ExpSquared";
                break;
            case FogMode::ExpSquared:
                fogMode = FogMode::Linear;
                newMode = "Linear";
                break;
            default:
                break;
            }

            std::cout << "Switched fog mode to: " << newMode << std::endl;
        }
        
        window.Update(dt);

    } while (!window.ShouldQuit());

    return 0;
}

