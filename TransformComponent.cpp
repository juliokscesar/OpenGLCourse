#include "TransformComponent.hpp"

#include <glm/gtc/matrix_transform.hpp>

void TransformComponent::SetPosition(const glm::vec3 &pos)
{
    m_position = pos;
    updateTransformMatrix();
}

void TransformComponent::Translate(const glm::vec3 &pos)
{
    SetPosition(m_position + pos);
}

void TransformComponent::SetRotation(const glm::vec3 &rot)
{
    m_rotation = rot;
    updateTransformMatrix();
}

void TransformComponent::Rotate(float deg, const glm::vec3& axis)
{
    float rad = glm::radians(deg);

    // TODO: implement rotation around other axis
    // this works if axis is (1,0,0) (0,1,0) or (0,0,1)
    // but i have no idea if it works around other axis
    glm::vec3 norm = glm::normalize(axis);
    glm::vec3 newRot = norm * rad; 

    SetRotation(newRot);
}

void TransformComponent::SetScale(const glm::vec3 &scale)
{
    m_scale = scale;
    updateTransformMatrix();
}

void TransformComponent::Scale(float factor)
{
    m_scale = glm::vec3(factor);
    updateTransformMatrix();
}

void TransformComponent::Update()
{
    updateTransformMatrix();
}

void TransformComponent::updateTransformMatrix()
{
    m_transMatrix = glm::mat4(1.0f);

    // Translation (relative to origin (0,0,0))
    m_transMatrix = glm::translate(m_transMatrix, m_position);

    // Rotation
    // TODO: QUATERNIONS
    float angleX = glm::radians(m_rotation.x);
    float angleY = glm::radians(m_rotation.y);
    float angleZ = glm::radians(m_rotation.z);

    // x-axis
    if (angleX)
        m_transMatrix = glm::rotate(m_transMatrix, angleX, glm::vec3(1.0f, 0.0f, 0.0f));
    // y-axis
    if (angleY)
        m_transMatrix = glm::rotate(m_transMatrix, angleY, glm::vec3(0.0f, 1.0f, 0.0f));
    // z-axis
    if (angleZ)
        m_transMatrix = glm::rotate(m_transMatrix, angleZ, glm::vec3(0.0f, 0.0f, 1.0f));

    // Scale
    m_transMatrix = glm::scale(m_transMatrix, m_scale);
}
