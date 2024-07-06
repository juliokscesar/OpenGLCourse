#pragma once

// Make sure to include glad.h before glfw
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>

class Window
{
public:
    Window(const char* title, uint32_t width, uint32_t height)
        : m_title(title), m_width(width), m_height(height) {}

    void Init();
    void MainLoop();

    GLFWwindow* GetGLFWWindow();

private:
    GLFWwindow* m_glfwWindow;
    uint32_t m_width, m_height;
    std::string m_title;

    void updateWindowProperties();
};
