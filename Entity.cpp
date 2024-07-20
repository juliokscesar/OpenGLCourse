#include "Entity.hpp"

Entity::Entity(const StaticMesh& mesh)
    : m_mesh(mesh)
{
}

void Entity::Update(float deltaTime)
{
    Transform.Update();
}

