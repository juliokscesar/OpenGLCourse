#include "Entity.hpp"

Entity::Entity()
    : m_mesh()
{
}

Entity::Entity(const Entity &other)
    : Transform(other.Transform), m_mesh(other.m_mesh), m_visible(other.m_visible)
{
}

Entity::Entity(const StaticMesh& mesh)
    : m_mesh(mesh)
{
}

void Entity::Update(float deltaTime)
{
    Transform.Update();
}

