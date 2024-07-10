#pragma once

#include <vector>

#include "StaticMesh.hpp"
#include "TransformComponent.hpp"
#include "Texture2D.hpp"

/*
    A class that represents anything that has a Mesh
    and a Transform Component
*/
class Entity
{
public:
    Entity()
        : Mesh(), Transform(), m_visible(false) {}

    Entity(const StaticMesh& mesh)
        : Mesh(mesh), Transform(), m_visible(true) {}

    Entity(const Entity& other);

    Entity& operator=(const Entity& other)
    {
        if (this != &other)
        {
            this->Mesh = other.Mesh;
            this->Transform = other.Transform;
            this->m_visible = other.m_visible;
        }
        return *this;
    }

    StaticMesh Mesh;
    TransformComponent Transform;

    // TODO: replace this with a Material component in StaticMesh class, not here
    //std::vector<Texture2D&> Textures;
    //float TexMixProportion;

    void Draw();

    void SetVisible(bool v);

private:
    bool m_visible;
};
