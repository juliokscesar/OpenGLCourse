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

// General light struct
struct Light
{
    vec3 position;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct DirectionalLight
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

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

//uniform Light light;
uniform DirectionalLight dirLight;
uniform PointLight pointLight;
uniform SpotLight spotLight;

uniform bool bEnableLighting;
uniform bool bUseDirectionalLight;
uniform bool bUsePointLight;
uniform bool bUseSpotLight;

void main()
{
    // Material texture
    vec3 texDiffuse  = vec3(texture(material.diffuseMap, TexCoord));
    vec3 texSpecular = vec3(texture(material.specularMap, TexCoord));

    vec3 resultColor;
    if (bEnableLighting)
    {
        vec3 ambientLight = vec3(0.0);
        vec3 diffuseLight = vec3(0.0);
        vec3 specularLight = vec3(0.0);

        if (bUseDirectionalLight)
        {
            // Ambient calculation
            ambientLight = dirLight.ambient * texDiffuse;

            // Diffuse calculation
            vec3 norm = normalize(Normal);
            vec3 lightDir = normalize(-dirLight.direction);
            float diff = max(dot(norm, lightDir), 0.0);
            diffuseLight = dirLight.diffuse * (diff * texDiffuse);

            // Specular calculation
            vec3 viewDir = normalize(viewPos - FragPos);
            vec3 reflectLightDir = reflect(-lightDir, norm);
            float spec = pow(max(dot(viewDir, reflectLightDir), 0.0), material.shininess);
            specularLight = dirLight.specular * (spec * texSpecular);
        }

        else if (bUsePointLight)
        {
            // Ambient calculation
            ambientLight = pointLight.ambient * texDiffuse;

            // Diffuse calculation
            vec3 norm = normalize(Normal);
            vec3 lightDir = normalize(pointLight.position - FragPos);
            float diff = max(dot(norm, lightDir), 0.0);
            diffuseLight = pointLight.diffuse * (diff * texDiffuse);

            // Specular calculation
            vec3 viewDir = normalize(viewPos - FragPos);
            vec3 reflectLightDir = reflect(-lightDir, norm);
            float spec = pow(max(dot(viewDir, reflectLightDir), 0.0), material.shininess);
            specularLight = pointLight.specular * (spec * texSpecular);

            // Point light attenuation calculation
            float dist = length(pointLight.position - FragPos);
            float attenuation = 1.0 / (pointLight.attConstant + pointLight.attLinear * dist + pointLight.attQuadratic * (dist * dist));
            ambientLight *= attenuation;
            diffuseLight *= attenuation;
            specularLight *= attenuation;
        }

        else if (bUseSpotLight)
        {
            vec3 fragLightDir = normalize(spotLight.position - FragPos);
            float cosTheta = dot(fragLightDir, normalize(-spotLight.direction));
            float cosOuter = cos(spotLight.outerCutoff);
            float cosInner = cos(spotLight.innerCutoff);
            float cosEpsilon = cosInner - cosOuter;
            float intensity = clamp((cosTheta - cosOuter) / cosEpsilon, 0.0, 1.0);

            // Ambient calculation
            ambientLight = spotLight.ambient * texDiffuse;

            // Diffuse calculation
            vec3 norm = normalize(Normal);
            float diff = max(dot(norm, fragLightDir), 0.0);
            diffuseLight = spotLight.diffuse * (diff * texDiffuse);

            // Specular calculation
            vec3 viewDir = normalize(viewPos - FragPos);
            vec3 reflected = reflect(-fragLightDir, norm);
            float spec = pow(max(dot(viewDir, reflected), 0.0), material.shininess);
            specularLight = spotLight.specular * (spec * texSpecular);

            // Smooth edges
            diffuseLight *= intensity;
            specularLight *= intensity;

            // Spotlight attenuation calculation
            float dist = length(spotLight.position - FragPos);
            float attenuation = 1.0 / (spotLight.attConstant + spotLight.attLinear * dist + spotLight.attQuadratic * (dist * dist));
            //ambientLight *= attenuation;
            diffuseLight *= attenuation;
            specularLight *= attenuation;
        }

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
