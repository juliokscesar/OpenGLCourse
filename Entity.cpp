#include "Entity.hpp"
#include "StaticMesh.hpp"

void DrawableEntity::Draw(const Shader& shader)
{
    shader.SetMat4("u_model", Transform.GetTransformMatrix());
}

void DrawableEntity::Update(float deltaTime)
{
    Transform.Update();
}


ModelEntity::ModelEntity(const Model& model)
    : m_model(model)
{
}

void ModelEntity::Draw(const Shader& shader)
{
    DrawableEntity::Draw(shader);

    m_model.Draw(shader);
}

void ModelEntity::Update(float deltaTime)
{
    DrawableEntity::Update(deltaTime);
}


StaticMeshEntity::StaticMeshEntity(const StaticMesh& staticMesh)
    : m_sMesh(staticMesh)
{
}

void StaticMeshEntity::Draw(const Shader& shader)
{
    DrawableEntity::Draw(shader);
    m_sMesh.Draw();
}

void StaticMeshEntity::Update(float deltaTime)
{
    DrawableEntity::Update(deltaTime);
}

