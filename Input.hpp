#pragma once

#include <GLFW/glfw3.h>

using glfwKeyCallbackFunc = void(*)(GLFWwindow* window, int key, int scancode, int action, int mods);

enum class KeyAction
{
    PRESS = GLFW_PRESS,
    RELEASE = GLFW_RELEASE,
    REPEAT = GLFW_REPEAT
};

namespace Input
{
    KeyAction GetKey(uint32_t key) noexcept;

    glfwKeyCallbackFunc GetInputCallback();
};
