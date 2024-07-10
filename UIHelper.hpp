#pragma once

#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

namespace UIHelper
{
    void Init(GLFWwindow* glfwWindow);
    void NewFrame();
    void Render();
    void Terminate();
}
