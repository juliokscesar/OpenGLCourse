#pragma once

#include "Shader.hpp"
#include "TransformComponent.hpp"
#include "ModelFactory.hpp"
#include "StaticMesh.hpp"

class DrawableEntity
{
public:
    virtual void Draw(const Shader& shader);
    virtual void Update(float deltaTime);

    virtual inline bool IsVisible() const noexcept { return m_visible; }
    virtual inline void SetVisible(bool visible) noexcept { m_visible = visible; }

    TransformComponent Transform;

private:
    bool m_visible = true;
};


class ModelEntity : public DrawableEntity
{
public:
    ModelEntity(const Model& model);

    void Draw(const Shader& shader) override;
    void Update(float deltaTime) override;

    inline Model& GetModel() noexcept { return m_model; }

private:
    Model m_model;
};


class StaticMeshEntity : public DrawableEntity
{
public:
    StaticMeshEntity(const StaticMesh& staticMesh);

    void Draw(const Shader& shader) override;
    void Update(float deltaTime) override;

    inline StaticMesh& GetMesh() noexcept { return m_sMesh; }

private:
    StaticMesh m_sMesh;
};

