#pragma once

#include "TransformComponent.hpp"
#include "StaticMesh.hpp"

class Entity
{
public:
    Entity();

    Entity(const Entity& other);

    Entity(const StaticMesh& mesh);

    virtual void Update(float deltaTime);

    inline StaticMesh& GetMeshRef() noexcept { return m_mesh; }
    
    inline bool IsVisible() const noexcept { return m_visible; }
    inline void SetVisible(bool visible) noexcept { m_visible = visible; }

public:
    TransformComponent Transform;

private:
    StaticMesh m_mesh;
    bool m_visible = true;
};

