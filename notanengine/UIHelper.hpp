#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <unordered_map>
#include <tuple>

#include "Entity.hpp"
#include "Light.hpp"
#include "Camera.hpp"


typedef std::unordered_map<std::string, std::tuple<Entity&, const Shader&>> EntityRenderMap;

namespace UIHelper
{
    void Init(GLFWwindow* glfwWindow);
    void NewFrame();
    void Render();
    void Terminate();

    void FrameStatsWindow(float deltaTime);

    void EntityPropertiesManager(const EntityRenderMap& entities);

    void DirectionalLightPropertiesManager(DirectionalLight& dirLight);

    void CameraAndProjectionPropertiesManager(Camera& camera, float& pNear, float& pFar);
}
