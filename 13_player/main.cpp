#include <vector>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <Window.h>
#include <Shader.h>
#include <HeightMap.h>
#include <Transform.h>
#include <TargaImage.h>
#include <Tree.h>
#include <FreetypeFont.h>
#include <Md2Model.h>
#include <GameTimer.h>
#include <Ogro.h>
#include <Explosion.h>
#include <GameWorld.h>
#include <MouseInterface.h>
#include <KeyboardInterface.h>

#ifdef __WIN32
#undef main
#endif

#define ANIM_POINTER(name) &Md2Model::Animation::name
const Md2Model::Animation* g_Animations[20] =
{
    ANIM_POINTER(Idle),
    ANIM_POINTER(Run),
    ANIM_POINTER(Attack),
    ANIM_POINTER(Pain1),
    ANIM_POINTER(Pain2),
    ANIM_POINTER(Pain3),
    ANIM_POINTER(Jump),
    ANIM_POINTER(FlipOff),
    ANIM_POINTER(Salute),
    ANIM_POINTER(Taunt),
    ANIM_POINTER(Wave),
    ANIM_POINTER(Point),
    ANIM_POINTER(CrouchIdle),
    ANIM_POINTER(CrouchWalk),
    ANIM_POINTER(CrouchAttack),
    ANIM_POINTER(CrouchPain),
    ANIM_POINTER(CrouchDeath),
    ANIM_POINTER(Death1),
    ANIM_POINTER(Death2),
    ANIM_POINTER(Death3)
};
#undef ANIM_POINTER
int g_NumAnims = 20;
int g_AnimIndex = 0;

// screen size
#define S_WIDTH 640
#define S_HEIGHT 480

inline void PrintFPS(GameTimer& t, FreetypeFont& f)
{
    static std::stringstream ss;
    ss.str(std::string());
    ss << "FPS: " << t.GetFPS();
    f.PrintString(ss.str(), 10, 480 - 10);
}

inline void PrintAnim(const std::string& name, FreetypeFont& f)
{
    static std::stringstream ss;
    ss.str(std::string());
    ss << "Anim: " << name;
    f.PrintString(ss.str(), 10, 50);
}

int main(int argc, char *argv[])
{
    Window            window;
    MouseInterface    mouseInterface;
    KeyboardInterface keyboardInterface;
    FreetypeFont      font("assets/LiberationSans-Regular.ttf", S_WIDTH, S_HEIGHT, 24);
    GameTimer         gameTimer;
    GameWorld         world;

    if (!window.Init("Hello World",
                    S_WIDTH, S_HEIGHT, 
                    &mouseInterface, 
                    &keyboardInterface)) {
        return 1;
    }
    GLuint gVAO = 0;
    glGenVertexArrays(1, &gVAO);
    glBindVertexArray(gVAO);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);

    srand(time(0));

    if (!font.Init()) {
        return 1;
    }
    if (!world.Init()) {
        return 1;
    }

    mouseInterface.SetWindow(&window);

    Mat4 projectionMatrix = perspective(52.0f,          // fov
                                        float(S_WIDTH)/float(S_HEIGHT),    // aspect
                                        1.0,            // near clip
                                        1000.0);         // far clip

    world.SetProjectionMatrix(&projectionMatrix);
    world.SetMouseInterface(&mouseInterface);
    world.SetKeyboardInterface(&keyboardInterface);
    world.SetScreenWidth(float(S_WIDTH));
    world.SetScreenHeight(float(S_HEIGHT));

    do
    {
        glClearColor(0.1, 0.2, 0.3, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        world.Render();

        PrintFPS(gameTimer, font);

        gameTimer.Update(1); // increase by 1 frame
        float dt = gameTimer.GetDeltaTimeSecs();
        world.Update(dt);
        window.Update(dt);

    } while (!window.ShouldQuit());

    return 0;
}
