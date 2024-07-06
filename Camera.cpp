#include "Camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include "Input.hpp"

void Camera::Update(float deltaTime)
{
    float speed = Speed * deltaTime;
    if (Input::GetKey(GLFW_KEY_W) == KeyAction::PRESS)
        Transform.Translate(m_front * speed);
    if (Input::GetKey(GLFW_KEY_S) == KeyAction::PRESS)
        Transform.Translate(-1.0f * m_front * speed);
    if (Input::GetKey(GLFW_KEY_D) == KeyAction::PRESS)
        Transform.Translate(glm::normalize(glm::cross(m_front, m_up)) * speed);
    if (Input::GetKey(GLFW_KEY_A) == KeyAction::PRESS)
        Transform.Translate(-1.0f * glm::normalize(glm::cross(m_front, m_up)) * speed);
}

glm::mat4 Camera::GetLookAtMatrix() const noexcept
{
    return glm::lookAt(Transform.GetPosition(), Transform.GetPosition() + m_front, m_up);
}
