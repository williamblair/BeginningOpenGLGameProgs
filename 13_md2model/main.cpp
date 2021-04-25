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

#ifdef __WIN32
#undef main
#endif

inline float deg2rad(const float deg) {
    return deg * M_PI / 180.0f;
}

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

class Ogro
{
public:
    Ogro(){}
    ~Ogro(){}
    
    bool Init()
    {
        if (!model.Load("assets/models/Ogro/tris.md2")) {
            return false;
        }
        if (!texture.Load("assets/models/Ogro/Ogrobase.tga")) {
            return false;
        }
        
        texID = Targa::Image::MakeTexture(texture);
        
        transform.rotation = angleAxis(deg2rad(-90.0f), Vec3(0.0f, 1.0f, 0.0f));
        transform.position.z = -2.0f;
        
        return true;
    }
    
    void Update(float dt)
    {
        model.Update(dt);
    }
    
    void Render(Shader& shader)
    {
        Mat4 modelMat = transformToMat4(transform);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texID);
        shader.SetUniform("uTexture0", 0);
        shader.SetUniform("uModel", modelMat);
        model.Render(shader);
    }
    
    inline std::string& GetAnimName() {
        return model.GetAnimName();
    }
    
    inline void SetAnim(const Md2Model::Animation* anim) {
        model.SetAnimation(*anim);
    }
    
private:
    Md2Model model;
    Targa::Image texture;
    GLuint texID;
    Transform transform;
};

class GameTimer
{
public:

    GameTimer() :
        lastTicks(0),
        thisTicks(0),
        ticksCounter(0),
        intervalTicks(1000*1),
        framesCount(0),
        deltaMilli(0),
        FPS(0.0f)
    {
        lastTicks = SDL_GetTicks();
    }
    
    inline float GetDeltaTimeMilli() const
    {
        return float(deltaMilli);
    }
    
    inline float GetDeltaTimeSecs() const
    {
        return GetDeltaTimeMilli()/1000.0f;
    }
    
    inline float GetFPS() const
    {
        return FPS;
    }

    void Update(int framesPassed)
    {
        framesCount += framesPassed;
        thisTicks = SDL_GetTicks();
        ticksCounter += thisTicks - lastTicks;
        deltaMilli = thisTicks - lastTicks;
        if (ticksCounter >= intervalTicks)
        {
            float secondsPassed = float(ticksCounter) / 1000.0f;
            FPS = float(framesCount) / secondsPassed;
            framesCount = 0;
            ticksCounter -= intervalTicks;
        }
        lastTicks = thisTicks;
    }

private:
    uint32_t lastTicks;
    uint32_t thisTicks;
    uint32_t ticksCounter;
    uint32_t intervalTicks;
    uint32_t framesCount;
    uint32_t deltaMilli;
    float FPS;
};

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
    Window window;
    Ogro ogro;
    Shader ogroShader;
    FreetypeFont font("assets/LiberationSans-Regular.ttf", 640, 480, 24);
    GameTimer gameTimer;

    if (!window.Initialize("Hello World", 640, 480)) {
        return 1;
    }
    GLuint gVAO = 0;
    glGenVertexArrays(1, &gVAO);
    glBindVertexArray(gVAO);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);
    
    ogroShader.Load("shaders/ogroVert.glsl", "shaders/ogroFrag.glsl");
    ogroShader.Bind();
    if (!font.Init()) {
        return 1;
    }
    if (!ogro.Init()) {
        return 1;
    }
    ogro.SetAnim(g_Animations[g_AnimIndex++]);
    
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

        ogroShader.Bind();
        UpdateUniforms(ogroShader);
        ogro.Render(ogroShader);

        PrintFPS(gameTimer, font);
        PrintAnim(ogro.GetAnimName(), font);
        
        if (window.SpaceKeyReleased()) {
            ogro.SetAnim(g_Animations[g_AnimIndex++]);
            if (g_AnimIndex >= g_NumAnims) {
                g_AnimIndex = 0;
            }
        }
        
        gameTimer.Update(1); // increase by 1 frame
        ogro.Update(gameTimer.GetDeltaTimeSecs());
        window.Update(gameTimer.GetDeltaTimeSecs());

    } while (!window.ShouldQuit());

#undef UpdateUniforms
    
    return 0;
}


