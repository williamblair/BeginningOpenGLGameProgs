#include <Application.h>

int main(int argc, char *argv[])
{
    Application app;
    if (!app.Initialize("Hello World", 640, 480)) {
        return 1;
    }

    Uint32 lastTicks = SDL_GetTicks();
    do
    {
        glClearColor(0.1, 0.2, 0.3, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        Uint32 thisTicks = SDL_GetTicks();
        float dt = thisTicks - lastTicks;
        lastTicks = thisTicks;
        
        app.Update(dt);
    } while (!app.ShouldQuit());

    return 0;
}

