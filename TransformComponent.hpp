#pragma once

#include <glm/glm.hpp>

/*
    A transform component contains position, scale and rotation.
    The transform matrix is stored and calculated through this.
*/
class TransformComponent
{
public:
    TransformComponent()
        : m_position(0.0f), m_rotation(0.0f), m_scale(1.0f), m_transMatrix(1.0f) {}
    
    inline glm::vec3 GetPosition() const noexcept { return m_position; }
    inline glm::vec3 GetRotation() const noexcept { return m_rotation; }
    inline glm::vec3 GetScale() const noexcept { return m_scale; }

    inline glm::mat4 GetTransformMatrix() const noexcept { return m_transMatrix; }

    void SetPosition(const glm::vec3& pos);
    void Translate(const glm::vec3& pos);

    void SetRotation(const glm::vec3& rot);
    void Rotate(float deg, const glm::vec3 axis);

    void SetScale(const glm::vec3& scale);
    void Scale(float factor);

    void Update();

private:
    glm::vec3 m_position;
    glm::vec3 m_rotation;
    glm::vec3 m_scale;

    glm::mat4 m_transMatrix;

    void updateTransformMatrix();
};
