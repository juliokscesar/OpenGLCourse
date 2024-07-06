#include "Entity.hpp"

void Entity::Draw()
{
    if (m_visible)
    {
        Mesh.Draw();
        Transform.Update();
        Mesh.GetShader().SetMat4("transform", Transform.GetTransformMatrix());
    }
}
