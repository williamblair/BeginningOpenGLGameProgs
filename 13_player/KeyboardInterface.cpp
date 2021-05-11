#include <KeyboardInterface.h>
#include <cstddef>
#include <iostream>

#define DBG_PRINT 0

KeyboardInterface::KeyboardInterface() :
    keysPressed(size_t(KeyboardCode::MAX_KEYS), false),
    keysHeld(size_t(KeyboardCode::MAX_KEYS), false)
{
}

KeyboardInterface::~KeyboardInterface()
{
}

bool KeyboardInterface::IsKeyPressed(KeyboardCode code)
{
    return keysPressed[int(code)];
}

bool KeyboardInterface::IsKeyHeldDown(KeyboardCode code)
{
    return keysHeld[int(code)];
}
    
void KeyboardInterface::SetKeyPressed(KeyboardCode code, bool isPressed)
{
    keysPressed[int(code)] = isPressed;
    
#if DBG_PRINT
    std::cout << "Set key pressed: " << int(code) << ", " << (isPressed ? "true" : "false") << std::endl;
#endif
}

void KeyboardInterface::SetKeyHeldDown(KeyboardCode code, bool isHeld)
{
    keysHeld[int(code)] = isHeld;

#if DBG_PRINT
    std::cout << "Set key held down: " << int(code) << ", " << (isHeld ? "true" : "false") << std::endl;
#endif
}

#undef DBG_PRINT
