#include "Input.hpp"

#define ACTION 0
#define MODS 1

#define N_KEYS 349

/*
    keyState[a][b]
    a: key
    b: 0=ACTION, 1=MODS
*/
static int keysActionMods[N_KEYS][2];
static bool keysState[N_KEYS];

struct MouseTracker
{
    float lastX = 0.0f;
    float lastY = 0.0f;
    float currX = 0.0f;
    float currY = 0.0f;

    float xOffset = 0.0f;
    float yOffset = 0.0f;

    bool bFirst = true;

    float scrollOffset = 0.0f;
};
static MouseTracker mouseTracker;

static void keyInputCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    keysActionMods[key][ACTION] = action;
    keysActionMods[key][MODS] = mods;

    if (action == GLFW_PRESS)
        keysState[key] = true;
    else if (action == GLFW_RELEASE)
        keysState[key] = false;
}

static void mouseMoveCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL)
        return;

    if (mouseTracker.bFirst)
    {
        mouseTracker.lastX = static_cast<float>(xpos);
        mouseTracker.lastY = static_cast<float>(ypos);
        mouseTracker.currX = mouseTracker.lastX;
        mouseTracker.currY = mouseTracker.lastY;

        mouseTracker.xOffset = 0.0f;
        mouseTracker.yOffset = 0.0f;

        mouseTracker.bFirst = false;

        return;
    }

    mouseTracker.lastX = mouseTracker.currX;
    mouseTracker.lastY = mouseTracker.currY;

    mouseTracker.currX = static_cast<float>(xpos);
    mouseTracker.currY = static_cast<float>(ypos);

    mouseTracker.xOffset = mouseTracker.currX - mouseTracker.lastX;
    mouseTracker.yOffset = mouseTracker.currY - mouseTracker.lastY;
}

static void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    mouseTracker.scrollOffset = static_cast<float>(yoffset);
}

int Input::GetKeyGLFWAction(int key) noexcept
{
    return keysActionMods[key][ACTION];
}

bool Input::GetKeyState(int key) noexcept
{
    return keysState[key];
}

void Input::Init(GLFWwindow* window)
{
    glfwSetKeyCallback(window, keyInputCallback);
    glfwSetCursorPosCallback(window, mouseMoveCallback);
    glfwSetScrollCallback(window, mouseScrollCallback);
}

float Input::GetMouseXOffset() noexcept
{
    if (mouseTracker.bFirst)
        return 0.0f;

    float offset = mouseTracker.xOffset;
    mouseTracker.xOffset = 0.0f;
    return offset;
}

float Input::GetMouseYOffset() noexcept
{
    if (mouseTracker.bFirst)
        return 0.0f;

    float offset = mouseTracker.yOffset;
    mouseTracker.yOffset = 0.0f;
    return offset;
}

float Input::GetMouseScroll() noexcept
{
    float scroll = mouseTracker.scrollOffset;
    mouseTracker.scrollOffset = 0.0f;
    return scroll;
}
