#include <Window.h>
#include <Shader.h>
#include <HeightMap.h>
#include <Transform.h>
#include <TargaImage.h>

inline float deg2rad(const float deg) {
    return deg * M_PI / 180.0f;
}

inline Mat4 MakeModelMat()
{
    Transform moveDown;
    Transform rotate;
    Transform moveBack;
    moveDown.position = Vec3(0.0f, -20.0f, 0.0f);
    rotate.rotation = angleAxis(deg2rad(25.0f), Vec3(1.0f, 0.0f, 0.0f));
    moveBack.position = Vec3(0.0f, 0.0f, -50.0f);
    
    Mat4 downMat = transformToMat4(moveDown);
    Mat4 rotateMat = transformToMat4(rotate);
    Mat4 moveBackMat = transformToMat4(moveBack);
    return downMat * rotateMat * moveBackMat;
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
    HeightMap heightMap;
    Targa::Image targaImage;

    if (!window.Initialize("Hello World", 640, 480)) {
        return 1;
    }

    GLuint gVAO = 0;
    glGenVertexArrays(1, &gVAO);
    glBindVertexArray(gVAO);

    glEnable(GL_DEPTH_TEST);

    shader.Load("shaders/vertexShader.glsl", "shaders/fragmentShader.glsl");
    shader.Bind();

    if (!heightMap.Load("assets/heightmap.raw")) {
        return 1;
    }
    if (!targaImage.Load("assets/grass.tga")) {
        return 1;
    }

    Mat4 projectionMatrix = perspective(52.0f,          // fov
                                        640.0/480.0,    // aspect
                                        1.0,            // near clip
                                        1000.0);         // far clip

    Mat4 viewMatrix; // = identity matrix

    Mat4 modelMatrix = MakeModelMat(); // = identity matrix

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

#define UpdateUniforms() \
    shader.SetUniform("uModel", modelMatrix);           \
    shader.SetUniform("uView", viewMatrix);             \
    shader.SetUniform("uProjection", projectionMatrix); \
    shader.SetUniform("uNormal", normalMatrix);         \
    shader.SetUniform("uTexture0", 0);                  \
    shader.SetUniform("uLightColor", lightColor);       \
    shader.SetUniform("uLightPos", lightPos);           \
    shader.SetUniform("uAmbientColor", ambientColor)

    GLuint texID = MakeTexture(targaImage);

    Uint32 lastTicks = SDL_GetTicks();
    do
    {
        glClearColor(0.1, 0.2, 0.3, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        UpdateUniforms();

        heightMap.Draw(shader);
        
        Uint32 thisTicks = SDL_GetTicks();
        float dt = thisTicks - lastTicks;
        lastTicks = thisTicks;

        UpdateLightPos();
        
        window.Update(dt);

    } while (!window.ShouldQuit());

    return 0;
}

