#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

uniform vec3 viewPos;

struct Material
{
    // vec3 ambient;
    // vec3 diffuse;
    sampler2D diffuseMap;
    sampler2D specularMap;
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

uniform bool bEnableLighting;

void main()
{
    // Material texture
    vec3 texDiffuse  = vec3(texture(material.diffuseMap, TexCoord));
    vec3 texSpecular = vec3(texture(material.specularMap, TexCoord));

    vec3 resultColor;
    if (bEnableLighting)
    {
        // Ambient calculation
        vec3 ambientLight = light.ambient * texDiffuse;

        // Diffuse calculation
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(light.position - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuseLight = light.diffuse * (diff * texDiffuse);

        // Specular calculation
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectLightDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectLightDir), 0.0), material.shininess);
        vec3 specularLight = light.specular * (spec * texSpecular);

        // Combined components of lighting
        vec3 phongLight = ambientLight + diffuseLight + specularLight;

        resultColor = phongLight;
    }
    else
    {
        resultColor = texDiffuse;
    }

    gl_FragColor = vec4(resultColor, 1.0);
}
