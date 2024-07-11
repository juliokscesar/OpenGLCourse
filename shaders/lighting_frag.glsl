#version 330 core

in vec3 FragNormal;
in vec3 FragPos;
in vec2 TexCoord;

uniform vec3 u_viewPos;

struct Material
{
    sampler2D diffuseMap;
    sampler2D specularMap;
    float shininess;
};
uniform Material u_material;


struct DirectionalLight
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform DirectionalLight u_dirLight;

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 diffMap, vec3 specMap)
{
    // Ambient light (just the ambient component of directional light)
    vec3 ambientLight = light.ambient * diffMap;

    // Diffuse light
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuseLight = light.diffuse * (diff * diffMap);

    // Specular light
    vec3 viewDir = normalize(u_viewPos - FragPos);
    vec3 reflectedLightDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectedLightDir), 0.0), u_material.shininess);
    vec3 specularLight = light.specular * (spec * specMap);

    return (ambientLight + diffuseLight + specularLight);
}

struct PointLight
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float attConstant;
    float attLinear;
    float attQuadratic;
};
uniform PointLight u_pointLight;

float LightAttenuation(float c, float linear, float quad, float dist)
{
    return (1.0 / (c + linear*dist + quad*dist*dist));
}

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 diffMap, vec3 specMap)
{
    float fragDistance = length(light.position - FragPos);
    float attenuation = LightAttenuation(light.attConstant, light.attLinear, light.attQuadratic, fragDistance);

    // Ambient light (again just the ambient component)
    vec3 ambientLight = light.ambient * diffMap;

    // Diffuse light
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuseLight = light.diffuse * (diff * diffMap);

    // Specular light
    vec3 viewDir = normalize(u_viewPos - FragPos);
    vec3 reflectedLightDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectedLightDir), 0.0), u_material.shininess);
    vec3 specularLight = light.specular * (spec * specMap);

    return ((ambientLight + diffuseLight + specularLight) * attenuation);
}

struct SpotLight
{
    vec3 position;
    vec3 direction;
    float innerCutoff; // in radians
    float outerCutoff; // in radians

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float attConstant;
    float attLinear;
    float attQuadratic;
};
uniform SpotLight u_spotLight;

vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 diffMap, vec3 specMap)
{
    vec3 fragLightDir = normalize(light.position - FragPos);

    // angle between fragLightDir and direction of spotlight
    float cosTheta = dot(fragLightDir, normalize(-light.direction));

    // outer cone cosine
    float cosOuter = cos(light.outerCutoff);
    // inner cone cosine
    float cosInner = cos(light.innerCutoff);

    // difference between outer and inner
    float cosEpsilon = cosInner - cosOuter;

    // smooth edge intensity formula
    float intensity = (cosTheta - cosOuter) / cosEpsilon;
    intensity = clamp(intensity, 0.0, 1.0);

    // Ambient component (not affected by inner/outer cone, only attenuation later)
    vec3 ambientLight = light.ambient * diffMap;

    // Diffuse component
    vec3 norm = normalize(normal);
    float diff = max(dot(norm, fragLightDir), 0.0);
    vec3 diffuseLight = light.diffuse * (diff * diffMap);

    // Specular component
    vec3 viewDir = normalize(u_viewPos - FragPos);
    vec3 reflectedLightDir = reflect(-fragLightDir, norm);
    float spec = pow(max(dot(viewDir, reflectedLightDir), 0.0), u_material.shininess);
    vec3 specularLight = light.specular * (spec * specMap);

    // apply intensity for smooth edges
    diffuseLight *= intensity;
    specularLight *= intensity;

    // calculate and apply attenuation
    float dist = length(fragLightDir);
    float attenuation = LightAttenuation(light.attConstant, light.attLinear, light.attQuadratic, dist);
    ambientLight *= attenuation;
    diffuseLight *= attenuation;
    specularLight *= attenuation;

    return (ambientLight + diffuseLight + specularLight);
}


uniform bool u_bEnableLighting;
uniform bool u_bUseDirectionalLight;
uniform bool u_bUsePointLight;
uniform bool u_bUseSpotLight;

void main()
{
    // Material texture
    vec3 texDiffuse  = vec3(texture(u_material.diffuseMap, TexCoord));
    vec3 texSpecular = vec3(texture(u_material.specularMap, TexCoord));

    vec3 resultColor = vec3(0.0);

    if (u_bEnableLighting)
    {
        if (u_bUseDirectionalLight)
        {
            resultColor += CalculateDirectionalLight(u_dirLight, FragNormal, texDiffuse, texSpecular);
        }

        if (u_bUsePointLight)
        {
            resultColor += CalculatePointLight(u_pointLight, FragNormal, texDiffuse, texSpecular);
        }

        if (u_bUseSpotLight)
        {
            resultColor += CalculateSpotLight(u_spotLight, FragNormal, texDiffuse, texSpecular);
        }
    }
    else
    {
        resultColor = texDiffuse;
    }
   
    gl_FragColor = vec4(resultColor, 1.0);
}
