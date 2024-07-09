#version 330 core

in vec3 Normal;
in vec3 FragPos;

uniform vec3 viewPos;

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
uniform Material material;

struct Light
{
    vec3 position;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;

void main()
{
    // Ambient calculation
    vec3 ambientLight = light.ambient * material.ambient;

    // Diffuse calculation
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuseLight = light.diffuse * (diff * material.diffuse);

    // Specular calculation
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectLightDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectLightDir), 0.0), material.shininess);
    vec3 specularLight = light.specular * (spec * material.specular);

    // Combined components of lighting
    vec3 phongLight = ambientLight + diffuseLight + specularLight;

    vec3 resultColor = phongLight;

    gl_FragColor = vec4(resultColor, 1.0);
}
