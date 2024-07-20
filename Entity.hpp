#pragma once

#include "TransformComponent.hpp"
#include "StaticMesh.hpp"

class Entity
{
public:
    Entity(const StaticMesh& mesh);

    virtual void Update(float deltaTime);

    inline StaticMesh& GetMeshRef() noexcept { return m_mesh; }

public:
    TransformComponent Transform;

private:
    StaticMesh m_mesh;
};

