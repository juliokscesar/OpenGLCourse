#version 330 core

#define MAX_NUMBER_SAMPLER2D 15

in vec3 FragPos;
in vec3 FragNormal;
in vec2 TexCoords;

struct Material
{
    sampler2D texture_diffuse[MAX_NUMBER_SAMPLER2D];
    sampler2D texture_specular[MAX_NUMBER_SAMPLER2D];
    float shininess;
};
uniform Material u_material;
uniform bool u_useMaterial;

uniform vec3 u_viewPos;


vec3 CalculateDiffuseLight(vec3 diffuseComponent, vec3 objDiffMap, vec3 normal, vec3 lightDir)
{
    vec3 norm = normalize(normal);
    vec3 dir = normalize(lightDir);
    float diff = max(dot(norm, dir), 0.0);

    vec3 diffuseLight = diffuseComponent * (diff * objDiffMap);

    return diffuseLight;
}

vec3 CalculateSpecularLight(vec3 specularComponent, vec3 objSpecMap, vec3 normal, vec3 lightDir)
{
    vec3 viewDir = normalize(u_viewPos - FragPos);
    vec3 dir = normalize(lightDir);

    vec3 reflectedLightDir = reflect(dir, normalize(normal));
    float spec = pow(max(dot(viewDir, reflectedLightDir), 0.0), u_material.shininess);

    vec3 specularLight = specularComponent * (spec * objSpecMap);
    return specularLight;
}

struct DirectionalLight
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform DirectionalLight u_dirLight;
uniform bool u_useDirectionalLight;

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 diffMap, vec3 specMap)
{
    vec3 ambientLight = light.ambient * diffMap;

    vec3 diffuseLight = CalculateDiffuseLight(light.diffuse, diffMap, normal, -light.direction);

    vec3 specularLight = CalculateSpecularLight(light.specular, specMap, normal, light.direction);

    return (ambientLight + diffuseLight + specularLight); 
}


float LightAttenuation(float c, float lin, float quad, float dist)
{
    return (1.0 / (c * lin*dist + quad*dist*dist));
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
uniform bool u_usePointLight;

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 diffMap, vec3 specMap)
{
    vec3 ambientLight = light.ambient * diffMap;


    vec3 lightDir = light.position - FragPos;

    vec3 diffuseLight = CalculateDiffuseLight(light.diffuse, diffMap, normal, lightDir);
    
    vec3 specularLight = CalculateSpecularLight(light.specular, specMap, normal, -lightDir);

    float fragDistance = length(lightDir);
    float attenuation = LightAttenuation(light.attConstant, light.attLinear, light.attQuadratic, fragDistance);

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
uniform bool u_useSpotLight;

vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 diffMap, vec3 specMap)
{
    vec3 fragLightDir = normalize(light.position - FragPos);

    // angle between fragLightDir and direction of spotlight
    float cosTheta = dot(fragLightDir, normalize(-light.direction));

    // outer cone cosine
    float cosOuter = cos(light.outerCutoff);
    // inner cone cosine
    float cosInner = cos(light.innerCutoff);

    float cosEpsilon = cosInner - cosOuter;

    // smooth edge intensity formula
    float intensity = (cosTheta - cosOuter) / cosEpsilon;
    intensity = clamp(intensity, 0.0, 1.0);

    
    vec3 ambientLight = light.ambient * diffMap;

    vec3 diffuseLight = CalculateDiffuseLight(light.diffuse, diffMap, normal, fragLightDir);

    vec3 specularLight = CalculateSpecularLight(light.specular, specMap, normal, -fragLightDir);

    // apply intensity for smooth edges
    diffuseLight *= intensity;
    specularLight *= intensity;

    // calculate and apply attenuation on return
    float dist = length(fragLightDir);
    float attenuation = LightAttenuation(light.attConstant, light.attLinear, light.attQuadratic, dist);

    return ((ambientLight + diffuseLight + specularLight) * attenuation);
}


uniform bool u_DEBUG_noRenderMaterial;

void main()
{
    vec3 resultColor = vec3(0.0);

    if (texture(u_material.texture_diffuse[0], TexCoords).a < 0.5)
	discard;

    // Test first with only one diffuse map and one specular map
    vec3 texDiffuse = vec3(texture(u_material.texture_diffuse[0], TexCoords));

    vec3 texSpecular = vec3(texture(u_material.texture_specular[0], TexCoords));


    if (u_useDirectionalLight)
        resultColor += CalculateDirectionalLight(u_dirLight, FragNormal, texDiffuse, texSpecular);

    if (u_usePointLight)
        resultColor += CalculatePointLight(u_pointLight, FragNormal, texDiffuse, texSpecular);

    if (u_useSpotLight)
        resultColor += CalculateSpotLight(u_spotLight, FragNormal, texDiffuse, texSpecular);


    if (u_DEBUG_noRenderMaterial)
	resultColor = vec3(1.0);

    gl_FragColor = vec4(resultColor, 1.0);
}

