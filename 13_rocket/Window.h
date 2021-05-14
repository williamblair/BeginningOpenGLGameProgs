#ifndef WINDOW_H_INCLUDED
#define WINDOW_H_INCLUDED

#include <iostream>
#include <cstdlib>
#include <map>

#include <GL/glew.h>
#include <SDL2/SDL.h>

#include <MouseInterface.h>
#include <KeyboardInterface.h>

class Window
{
public:

    inline Window() :
        window(nullptr),
        mouseInterface(nullptr),
        keyboardInterface(nullptr),
        quit(false),
        spaceKeyReleased(false)
    {
    }
    inline virtual ~Window() {
        Shutdown();
    }
    inline virtual bool Init(const char* title,
                             const int width, 
                             const int height, 
                             MouseInterface* mouseInterface, 
                             KeyboardInterface* keyboardInterface) {
        
        this->mouseInterface = mouseInterface;
        this->keyboardInterface = keyboardInterface;
        
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cerr << __func__ << ": failed to init SDL: " << SDL_GetError() << std::endl;
            return false;
        }

        window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
        if (!window) {
            std::cerr << __func__ << ": failed to create window: " << SDL_GetError() << std::endl;
            return false;
        }
    
        // set opengl options
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    
        // create a render context
        gc = SDL_GL_CreateContext(window);
    
        // enable vsync
        SDL_GL_SetSwapInterval(1);
    
        // init glew
        GLenum err = glewInit();
        if (err != GLEW_OK) {
            std::cerr << __func__ << "Error initializing GLEW: " << glewGetErrorString(err) << std::endl;
            return false;
        } else {
            std::cout << "Init GLEW, version: " << glewGetString(GLEW_VERSION) << std::endl;
        }

        return true;
    }
    inline virtual void Update(float inDeltaTime) {
        SDL_Event e;
        spaceKeyReleased = false;
        for (int i = 0; i < int(KeyboardCode::MAX_KEYS); ++i) {
            KeyboardCode k = (KeyboardCode)i;
            keyboardInterface->SetKeyPressed(k, false);
        }
        mouseInterface->SetButtonPressed(0, false);
        mouseInterface->SetButtonPressed(1, false);
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN) {
                switch (e.button.button)
                {
                case SDL_BUTTON_LEFT:
                    mouseInterface->SetButtonPressed(0, false);
                    break;
                case SDL_BUTTON_RIGHT:
                    mouseInterface->SetButtonPressed(1, false);
                    break;
                }
            }
            else if (e.type == SDL_MOUSEBUTTONUP) {
                switch (e.button.button)
                {
                case SDL_BUTTON_LEFT:
                    mouseInterface->SetButtonPressed(0, true);
                    break;
                case SDL_BUTTON_RIGHT:
                    mouseInterface->SetButtonPressed(1, true);
                    break;
                }
            }
            else if (e.type == SDL_MOUSEMOTION) {
                int mouseX = e.motion.x;
                int mouseY = e.motion.y;
                mouseInterface->SetMousePos(mouseX, mouseY);
            }
            else if (e.type == SDL_KEYDOWN) {
                KeyboardCode k;
                if (SDLKeyToKeyboardCode(e.key.keysym.sym, k)) {
                    keyboardInterface->SetKeyHeldDown(k, true);
                    keyboardInterface->SetKeyPressed(k, false);
                }
            }
            else if (e.type == SDL_KEYUP) {
                KeyboardCode k;
                if (SDLKeyToKeyboardCode(e.key.keysym.sym, k)) {
                    keyboardInterface->SetKeyHeldDown(k, false);
                    keyboardInterface->SetKeyPressed(k, true);
                }
            }
        }

        SDL_GL_SwapWindow(window);
    }
    inline virtual void Render(float inAspectRatio) {}
    inline virtual void Shutdown() {
        if (window != nullptr) {
           SDL_GL_DeleteContext(gc);
           SDL_DestroyWindow(window);
           SDL_Quit();
           window = nullptr;
        }
    }
    inline virtual bool ShouldQuit() const {
        return quit;
    }
    inline virtual bool SpaceKeyReleased() const {
        return spaceKeyReleased;
    }
private:

    // disallow copy
    Window(const Window&);
    Window& operator=(const Window&);

    SDL_Window* window;
    SDL_GLContext gc;
    MouseInterface* mouseInterface;
    KeyboardInterface* keyboardInterface;
    bool quit;
    bool spaceKeyReleased;
    
    inline bool SDLKeyToKeyboardCode(SDL_Keycode sdlKey, KeyboardCode& res)
    {
        static std::map<SDL_Keycode, KeyboardCode> keyMap = {
            { SDLK_UP, KeyboardCode::UP },
            { SDLK_DOWN, KeyboardCode::DOWN },
            { SDLK_LEFT, KeyboardCode::LEFT },
            { SDLK_RIGHT, KeyboardCode::RIGHT },
            { SDLK_w, KeyboardCode::W },
            { SDLK_a, KeyboardCode::A },
            { SDLK_s, KeyboardCode::S },
            { SDLK_d, KeyboardCode::D },
            { SDLK_SPACE, KeyboardCode::SPACE }
        };
        
        if (keyMap.count(sdlKey) > 0) {
            res = keyMap[sdlKey];
            return true;
        }
        
        return false;
    }
};

#endif // WINDOW_H_INCLUDED

