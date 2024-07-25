#pragma once

#include <unordered_map>
#include <string>

#include "Entity.hpp"
#include "Shader.hpp"
#include "Camera.hpp"

typedef std::unordered_map<std::string, std::tuple<Entity&, const Shader&>> EntityRenderMap;

namespace Render
{
    void UpdateAndDrawEntity(Entity& entity, const Shader& shader, float deltaTime, const Camera& camera, const glm::mat4& projection);

    void UpdateAndDrawEntityMap(const EntityRenderMap& entities, float deltaTime, const Camera& camera, const glm::mat4& projection);
}
