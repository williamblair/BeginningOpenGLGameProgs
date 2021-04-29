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
    Window          window;
    Shader          ogroShader;
    FreetypeFont    font("assets/LiberationSans-Regular.ttf", 640, 480, 24);
    GameTimer       gameTimer;
    GameWorld       world;

    if (!window.Initialize("Hello World", 640, 480)) {
        return 1;
    }
    GLuint gVAO = 0;
    glGenVertexArrays(1, &gVAO);
    glBindVertexArray(gVAO);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);

    srand(time(0));

    ogroShader.Load("shaders/ogroVert.glsl", "shaders/ogroFrag.glsl");
    ogroShader.Bind();
    if (!font.Init()) {
        return 1;
    }
    if (!world.Init()) {
        return 1;
    }

    Mat4 projectionMatrix = perspective(52.0f,          // fov
                                        640.0/480.0,    // aspect
                                        1.0,            // near clip
                                        1000.0);         // far clip

    Mat4 viewMatrix; // = identity matrix

#define UpdateUniforms(shader)              \
    shader.SetUniform("uView", viewMatrix); \
    shader.SetUniform("uProjection", projectionMatrix)

    do
    {
        glClearColor(0.1, 0.2, 0.3, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        PrintFPS(gameTimer, font);

        gameTimer.Update(1); // increase by 1 frame
        float dt = gameTimer.GetDeltaTimeSecs();
        world.Update(dt);
        window.Update(dt);

    } while (!window.ShouldQuit());

#undef UpdateUniforms

    return 0;
}
