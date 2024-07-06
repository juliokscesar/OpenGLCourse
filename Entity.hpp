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
    Entity(const StaticMesh& mesh)
        : Mesh(mesh), Transform(), m_visible(true) {}

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
