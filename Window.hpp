#pragma once

// Make sure to include glad.h before glfw
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>

class Window
{
public:
    Window(const char* title, uint32_t width, uint32_t height)
        : m_width(width), m_height(height), m_aspectRatio((float)m_width / (float)m_height), m_title(title) {}

    void Init();
    void MainLoop();
    void Finish();

    inline GLFWwindow* GetGLFWWindow() const noexcept { return m_glfwWindow; }

private:
    GLFWwindow* m_glfwWindow;
    uint32_t m_width, m_height;
    float m_aspectRatio;
    std::string m_title;

    void updateWindowProperties();
};
