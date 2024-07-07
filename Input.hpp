#pragma once

#include <GLFW/glfw3.h>

using glfwKeyCallbackFunc = void(*)(GLFWwindow* window, int key, int scancode, int action, int mods);

enum class KeyAction
{
    PRESS   = GLFW_PRESS,
    RELEASE = GLFW_RELEASE,
    REPEAT  = GLFW_REPEAT
};

namespace Input
{
    KeyAction GetKeyGLFWAction(int key) noexcept;

    bool GetKeyState(int key) noexcept;

    void Init(GLFWwindow* window);

    void AddKeyListener(int key);

    void AddKeyListener(int keys[], size_t n);

    float GetMouseXOffset() noexcept;
    float GetMouseYOffset() noexcept;
    float GetMouseScroll()  noexcept;
};
