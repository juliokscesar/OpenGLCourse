#pragma once

#include "TransformComponent.hpp"

class Camera
{
public:
    Camera()
        :   Transform(), 
            m_front(0.0f, 0.0f, 1.0f), 
            m_up(0.0f, 1.0f, 0.0f), 
            m_yaw(-90.0f),
            m_pitch(0.0f),
            Speed(1.0f), 
            Sensitivity(1.0f),
            FOV(45.0f) {}

    void Update(float deltaTime);

    glm::mat4 GetLookAtMatrix() const noexcept;

    TransformComponent Transform;

    float Speed;
    float Sensitivity;

    float FOV;

private:
    glm::vec3 m_front;
    glm::vec3 m_up;

    float m_yaw   = -90.0f;
    float m_pitch = 0.0f;
};
