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
    
    TransformComponent(const TransformComponent& other)
        : m_position(other.m_position), m_rotation(other.m_rotation), m_scale(other.m_scale), m_transMatrix(1.0f) {}

    TransformComponent& operator=(const TransformComponent& other)
    {
        if (this != &other)
        {
            this->m_position = other.m_position;
            this->m_rotation = other.m_rotation;
            this->m_scale = other.m_scale;
        }
        this->updateTransformMatrix();
        return *this;
    }

    inline glm::vec3 GetPosition() const noexcept { return m_position; }
    inline glm::vec3 GetRotation() const noexcept { return m_rotation; }
    inline glm::vec3 GetScale() const noexcept { return m_scale; }

    inline glm::mat4 GetTransformMatrix() const noexcept { return m_transMatrix; }

    void SetPosition(const glm::vec3& pos);
    void Translate(const glm::vec3& pos);
    inline glm::vec3& GetPositionRef() noexcept { return m_position; }

    void SetRotation(const glm::vec3& rot);
    void Rotate(float deg, const glm::vec3& axis);
    inline glm::vec3& GetRotationRef() noexcept { return m_rotation; }

    void SetScale(const glm::vec3& scale);
    void Scale(float factor);
    inline glm::vec3& GetScaleRef() noexcept { return m_scale; }

    void Update();

private:
    glm::vec3 m_position;
    glm::vec3 m_rotation;
    glm::vec3 m_scale;

    glm::mat4 m_transMatrix;

    void updateTransformMatrix();
};
