#include <cassert>
#include <MouseInterface.h>
#include <Window.h>


//#define DBG_ASSERT(cond)
#define DBG_ASSERT(cond) assert((cond))

#define DBG_PRINT 0

MouseInterface::MouseInterface(const int screenWidth,
                               const int screenHeight, 
                               Window* window)
{
    mouseButtons[0] = false;
    mouseButtons[1] = false;
    mousePos[0] = screenWidth / 2;
    mousePos[1] = screenHeight / 2;
    this->window = window;
}

MouseInterface::MouseInterface()
{
    mouseButtons[0] = false;
    mouseButtons[1] = false;
    mousePos[0] = 0;
    mousePos[1] = 0;
    window = nullptr;
}

MouseInterface::~MouseInterface()
{
}

void MouseInterface::GetMousePos(int& x, int& y)
{
    x = mousePos[0];
    y = mousePos[1];
}

void MouseInterface::SetMousePos(int x, int y)
{
    mousePos[0] = x;
    mousePos[1] = y;
    // TODO - set cursor position on screen?

#if DBG_PRINT
    std::cout << __func__ << ": mouse pos: " 
              << mousePos[0] << ", "
              << mousePos[1] << std::endl;
#endif
}

void MouseInterface::ShowCursor(bool isShown)
{
    // TODO
}

void MouseInterface::SetButtonPressed(int button, bool isPressed)
{
    DBG_ASSERT(button == 0 || button == 1);
    mouseButtons[button] = isPressed;
    
#if DBG_PRINT
    std::cout << __func__ << ": mouse button pressed: " 
              << button << ": " << isPressed 
              << std::endl;
#endif
}

bool MouseInterface::IsButtonPressed(int button)
{
    DBG_ASSERT(button == 0 || button == 1);
    return mouseButtons[button];
}

#undef DBG_ASSERT
#undef DBG_PRINT
