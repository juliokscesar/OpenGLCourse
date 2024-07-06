#include "Input.hpp"

#define ACTION 0
#define MODS 1

/*
    keyState[a][b]
    a: key action
    b: key mods
*/
static uint32_t keyState[349][2];

void keyInputCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    keyState[key][ACTION] = action;
    keyState[key][MODS] = mods;
}

KeyAction Input::GetKey(uint32_t key) noexcept
{
    return static_cast<KeyAction>(keyState[key][ACTION]);
}

glfwKeyCallbackFunc Input::GetInputCallback()
{
    return keyInputCallback;
}
