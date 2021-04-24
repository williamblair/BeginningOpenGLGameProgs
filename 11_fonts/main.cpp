#include <vector>
#include <cstdlib>
#include <ctime>
#include <Window.h>
#include <Shader.h>
#include <HeightMap.h>
#include <Transform.h>
#include <TargaImage.h>
#include <Tree.h>
#include <FreetypeFont.h>

#ifdef __WIN32
#undef main
#endif

inline float deg2rad(const float deg) {
    return deg * M_PI / 180.0f;
}

int main(int argc, char *argv[])
{
    Window window;
    FreetypeFont font("assets/LiberationSans-Regular.ttf", 640, 480, 36);

    if (!window.Initialize("Hello World", 640, 480)) {
        return 1;
    }
    if (!font.Init()) {
        return 1;
    }


    GLuint gVAO = 0;
    glGenVertexArrays(1, &gVAO);
    glBindVertexArray(gVAO);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);

    Uint32 lastTicks = SDL_GetTicks();
    do
    {
        glClearColor(0.1, 0.2, 0.3, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Input coordinates are bottom left
        font.PrintString("Hello World", 10, 100);

        Uint32 thisTicks = SDL_GetTicks();
        float dt = thisTicks - lastTicks;
        lastTicks = thisTicks;
        
        window.Update(dt);

    } while (!window.ShouldQuit());

    return 0;
}

