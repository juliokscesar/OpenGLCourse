#pragma once

#include <unordered_map>
#include <string>

#include "Entity.hpp"
#include "Shader.hpp"
#include "Camera.hpp"

typedef std::unordered_map<std::string, std::tuple<Entity&, const Shader&>> EntityRenderMap;

namespace Render
{
    void DrawMeshData(MeshData& meshData);

    void DrawStaticMesh(StaticMesh& mesh);

    // stencil buffer test
    void DrawOutlineStaticMesh(StaticMesh& mesh, const Shader& defaultShader, const Shader& outlineShader, const glm::vec3& outlineColor);
    
    void DrawEntity(Entity& entity, const Shader& shader, const Camera& camera, const glm::mat4& projection);

    void DrawOutlineEntity(Entity& entity, const Shader& defaultShader, const Shader& outlineShader, const glm::vec3& outlineColor, const Camera& camera, const glm::mat4& projection, float outlineFactor=1.1f);

    void UpdateAndDrawEntity(Entity& entity, const Shader& shader, float deltaTime, const Camera& camera, const glm::mat4& projection);
    void UpdateAndDrawEntityMap(const EntityRenderMap& entities, float deltaTime, const Camera& camera, const glm::mat4& projection);
}
