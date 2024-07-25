#include "Camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include "Input.hpp"

void Camera::Update(float deltaTime)
{
    // ===== WASD MOVEMENT =====
    float speed = Speed * deltaTime;
    if (Input::GetKeyState(GLFW_KEY_W))
        Transform.Translate(m_front * speed);

    if (Input::GetKeyState(GLFW_KEY_S))
        Transform.Translate(-1.0f * m_front * speed);

    if (Input::GetKeyState(GLFW_KEY_D))
        Transform.Translate(glm::normalize(glm::cross(m_front, m_up)) * speed);
        
    if (Input::GetKeyState(GLFW_KEY_A))
        Transform.Translate(-1.0f * glm::normalize(glm::cross(m_front, m_up)) * speed);

    if (Input::GetKeyState(GLFW_KEY_SPACE))
        Transform.Translate(m_up * speed);
    
    if (Input::GetKeyState(GLFW_KEY_LEFT_SHIFT))
        Transform.Translate(m_up * speed * -1.0f);
    // ===== END WASD MOVEMENT =====


    // ===== MOUSE MOVEMENT LOOK =====
    // Pitch: rotate around X-axis, Yaw: rotate around Y-axis
    float mouseXOffset = Input::GetMouseXOffset() * Sensitivity;
    float mouseYOffset = -1.0f * Input::GetMouseYOffset() * Sensitivity;

    m_yaw   += mouseXOffset;
    m_pitch += mouseYOffset;

    // constrain pitch to -89.0f - +89.0f
    if (m_pitch > 89.0f)
        m_pitch = 89.0f;
    else if (m_pitch < -89.0f)
        m_pitch = -89.0f;

    // constrain yaw to max(abs(yaw)) = 360.0f
    if (m_yaw >= 360.0f || m_yaw <= -360.0f)
        m_yaw = 0.0f;

    glm::vec3 direction{};
    direction.x = std::cos(glm::radians(m_yaw)) * std::cos(glm::radians(m_pitch));
    direction.y = std::sin(glm::radians(m_pitch));
    direction.z = std::sin(glm::radians(m_yaw)) * std::cos(glm::radians(m_pitch));
    m_front = glm::normalize(direction);
    // ===== END MOUSE MOVEMENT LOOK =====

    // ===== ZOOM =====
    float scroll = Input::GetMouseScroll();
    if (scroll)
        FOV -= scroll; // subtract because otherwise the scrolling is inverted
    
    // Clamp FOV to 1.0 and 45.0
    if (FOV < 1.0f)
        FOV = 1.0f;
    else if (FOV > 45.0f)
        FOV = 45.0f;
}

glm::mat4 Camera::GetLookAtMatrix() const noexcept
{
    return glm::lookAt(Transform.GetPosition(), Transform.GetPosition() + m_front, m_up);
}
