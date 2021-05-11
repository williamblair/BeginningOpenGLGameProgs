#ifndef KEYBOARD_INTERFACE_H_INCLUDED
#define KEYBOARD_INTERFACE_H_INCLUDED

#include <vector>

enum class KeyboardCode
{
    INVALID = 0,
    UP = 1,
    DOWN = 2,
    LEFT = 3,
    RIGHT = 4,
    SPACE = 5,
    W = 6,
    A = 7,
    S = 8,
    D = 9,
    MAX_KEYS = 10
};

class KeyboardInterface
{
public:
    KeyboardInterface();
    ~KeyboardInterface();
    bool IsKeyPressed(KeyboardCode code);
    bool IsKeyHeldDown(KeyboardCode code);
    
    void SetKeyPressed(KeyboardCode code, bool isPressed);
    void SetKeyHeldDown(KeyboardCode code, bool isHeld);

private:
    std::vector<bool> keysPressed;
    std::vector<bool> keysHeld;
};

#endif // KEYBOARD_INTERFACE_H_INCLUDED
