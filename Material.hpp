#pragma once

#include <glm/glm.hpp>

struct SimpleMaterial
{
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};

struct MaterialMap
{
    int diffuseMapTex;
    int specularMapTex;
    float shininess;
};
