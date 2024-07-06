#include "Input.hpp"

#define ACTION 0
#define MODS 1

#define N_KEYS 349

/*
    keyState[a][b]
    a: key action
    b: key mods
*/
static int keysActionMods[N_KEYS][2];
static bool keysState[N_KEYS];

// USE A KEY BUFFER THAT UPDATES WHEN A KEY IS PRESSED OR RELEASED,
// THEN SET AN ARRAY WITH KEYS TO LISTEN, TO AVOID LOOPING EVERY FRAME

void keyInputCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    keysActionMods[key][ACTION] = action;
    keysActionMods[key][MODS] = mods;

    if (action == GLFW_PRESS)
        keysState[key] = true;
    else if (action == GLFW_RELEASE)
        keysState[key] = false;
}

KeyAction Input::GetKeyGLFWAction(int key) noexcept
{
    return static_cast<KeyAction>(keysActionMods[key][ACTION]);
}

bool Input::GetKeyState(int key) noexcept
{
    return keysState[key];
}

void Input::Init(GLFWwindow* window)
{
    glfwSetKeyCallback(window, keyInputCallback);
}

