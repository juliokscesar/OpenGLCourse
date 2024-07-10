#pragma once

#include "TransformComponent.hpp"

class Camera
{
public:
    Camera()
        :   m_front(0.0f, 0.0f, 1.0f), 
            m_up(0.0f, 1.0f, 0.0f) {}

    void Update(float deltaTime);

    glm::mat4 GetLookAtMatrix() const noexcept;

    inline glm::vec3 GetFrontVector() const noexcept { return m_front; }

public:
    TransformComponent Transform;

    float Speed = 1.0f;
    float Sensitivity = 1.0f;

    float FOV = 45.0f;

private:
    glm::vec3 m_front;
    glm::vec3 m_up;

    float m_yaw   = -90.0f;
    float m_pitch = 0.0f;
};
