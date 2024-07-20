#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <unordered_map>

#include "Entity.hpp"

namespace UIHelper
{
    void Init(GLFWwindow* glfwWindow);
    void NewFrame();
    void Render();
    void Terminate();

    void FrameStatsWindow(float deltaTime);

    void EntityPropertiesManager(const std::unordered_map<std::string, DrawableEntity&>& entities);
}
