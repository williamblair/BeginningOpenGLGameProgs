#ifndef MOUSE_INTERFACE_H_INCLUDED
#define MOUSE_INTERFACE_H_INCLUDED

#include <SDL2/SDL.h>

// forward declaration
class Window;

class MouseInterface
{
public:
    
    MouseInterface();
    MouseInterface(const int screenWidth, const int screenHeight, Window* window);
    ~MouseInterface();
    
    void SetWindow(Window* window) { this->window = window; }
    void SetScreenWidth(const int screenWidth) { mousePos[0] = screenWidth / 2; }
    void SetScreenHeight(const int screenHeight) { mousePos[1] = screenHeight / 2; }
    
    void GetMousePos(int& x, int& y);
    void SetMousePos(int x, int y);
    
    void ShowCursor(bool isShown);
    
    void SetButtonPressed(int button, bool isPressed);
    bool IsButtonPressed(int button);
    
private:
    
    bool mouseButtons[2]; // true if pressed down last update call
    int mousePos[2]; // x and y of last updated mouse position
    
    Window* window;
};

#endif // MOUSE_INTERFACE_H_INCLUDED
