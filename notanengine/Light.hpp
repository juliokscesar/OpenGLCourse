#pragma once

#include <glm/glm.hpp>

#include <string>

#include "Shader.hpp"

struct AttenuationProperties
{
    AttenuationProperties()
        : Constant(1.0f), Linear(0.0f), Quadratic(0.0f) {}

    AttenuationProperties(float attConst, float attLinear, float attQuadratic)
        : Constant(attConst), Linear(attLinear), Quadratic(attQuadratic) {}

    float Constant;
    float Linear;
    float Quadratic;
};


struct Light
{
    Light()
        : Ambient(0.0f), Diffuse(0.0f), Specular(0.0f) {}

    Light(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular)
        : Ambient(ambient), Diffuse(diffuse), Specular(specular) {}

    glm::vec3 Ambient;
    glm::vec3 Diffuse;
    glm::vec3 Specular;

    std::string UniformName;

    virtual void SetLightUniforms(const Shader& shader);
};

struct DirectionalLight : public Light
{
    DirectionalLight()
        : Light(), Direction(0.0f, -1.0f, 0.0f) {}

    DirectionalLight(const glm::vec3& direction, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular)
        : Light(ambient, diffuse, specular), Direction(direction) {}

    glm::vec3 Direction;

    void SetLightUniforms(const Shader& shader) override;
};

struct PointLight : public Light
{
    PointLight()
        : Light(), Position(0.0f), Attenuation() {}

    PointLight(const glm::vec3& position, float attConstant, float attLinear, float attQuadratic)
        : Light(), Position(position), Attenuation(attConstant, attLinear, attQuadratic) {}

    PointLight(const glm::vec3& position, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float attConstant, float attLinear, float attQuadratic)
        : Light(ambient, diffuse, specular), Position(position), Attenuation(attConstant, attLinear, attQuadratic) {}

    glm::vec3 Position;

    AttenuationProperties Attenuation;
    
    void SetLightUniforms(const Shader& shader) override;
};

struct SpotLight : public Light
{
    SpotLight()
        : Light(), Position(0.0f), Direction(0.0f, -1.0f, 0.0f), InnerCutoff(15.0f), OuterCutoff(17.0f), Attenuation() {}

    SpotLight(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float innerCutoff, float outerCutoff, float attConstant, float attLinear, float attQuadratic)
        : Light(ambient, diffuse, specular), Position(position), Direction(direction), InnerCutoff(innerCutoff), OuterCutoff(outerCutoff), Attenuation(attConstant, attLinear, attQuadratic) {}

    glm::vec3 Position;
    glm::vec3 Direction;

    float InnerCutoff;
    float OuterCutoff;

    AttenuationProperties Attenuation;

    void SetLightUniforms(const Shader& shader) override;
};
