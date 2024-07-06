#include "Entity.hpp"

Entity::Entity(const Entity &other)
{
    this->Mesh      = other.Mesh;
    this->Transform = other.Transform;
    this->m_visible = other.m_visible;
}

void Entity::Draw()
{
    if (m_visible)
    {
        Mesh.Draw();
        Transform.Update();
        Mesh.GetShader().SetMat4("transform", Transform.GetTransformMatrix());
    }
}
