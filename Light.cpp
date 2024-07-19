#include "Light.hpp"

void Light::SetLightUniforms(const Shader& shader)
{
    shader.SetVec3(UniformName + ".ambient", Ambient);
    shader.SetVec3(UniformName + ".diffuse", Diffuse);
    shader.SetVec3(UniformName + ".specular", Specular);
}


void DirectionalLight::SetLightUniforms(const Shader& shader)
{
    Light::SetLightUniforms(shader);

    shader.SetVec3(UniformName + ".direction", Direction);
}


void PointLight::SetLightUniforms(const Shader& shader)
{
    Light::SetLightUniforms(shader);

    shader.SetVec3(UniformName + ".position", Position);

    shader.SetFloat(UniformName + ".attConstant", Attenuation.Constant);
    shader.SetFloat(UniformName + ".attLinear", Attenuation.Linear);
    shader.SetFloat(UniformName + ".attQuadratic", Attenuation.Quadratic);
}


void SpotLight::SetLightUniforms(const Shader& shader)
{
    Light::SetLightUniforms(shader);

    shader.SetVec3(UniformName + ".position", Position);
    shader.SetVec3(UniformName + ".direction", Direction);

    shader.SetFloat(UniformName + ".innerCutoff", glm::radians(InnerCutoff));
    shader.SetFloat(UniformName + ".outerCutoff", glm::radians(OuterCutoff));

    shader.SetFloat(UniformName + ".attConstant", Attenuation.Constant);
    shader.SetFloat(UniformName + ".attLinear", Attenuation.Linear);
    shader.SetFloat(UniformName + ".attQuadratic", Attenuation.Quadratic);
}

