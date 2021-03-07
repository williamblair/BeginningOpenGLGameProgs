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

    glUniformMatrix4fv(shader.GetUniform("uModel"), 1, false, (const GLfloat*)modelMatrix.v);
    glUniformMatrix4fv(shader.GetUniform("uView"), 1, false, (const GLfloat*)viewMatrix.v);
    glUniformMatrix4fv(shader.GetUniform("uProjection"), 1, false, (const GLfloat*)projectionMatrix.v);
    glUniform1i(shader.GetUniform("uTexture0"), 0); // texture 0

    GLuint texID = MakeTexture(targaImage);

    Uint32 lastTicks = SDL_GetTicks();
    do
    {
        glClearColor(0.1, 0.2, 0.3, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        heightMap.Draw(shader);
        
        Uint32 thisTicks = SDL_GetTicks();
        float dt = thisTicks - lastTicks;
        lastTicks = thisTicks;
        
        window.Update(dt);

        SDL_Delay(1000.0/60.0);
    } while (!window.ShouldQuit());

    return 0;
}

