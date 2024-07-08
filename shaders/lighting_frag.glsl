#version 330 core

in vec3 Normal;
in vec3 FragPos;

uniform vec3 objectColor;

uniform vec3 lightColor;
uniform vec3 lightPos;

uniform vec3 viewPos;

uniform float ambientFactor;

uniform float specularStrength;
uniform float specularShininess;

void main()
{
    // Ambient calculation
    vec3 ambientLight = ambientFactor * lightColor;

    // Diffuse calculation
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuseLight = diff * lightColor;

    // Specular calculation
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectLightDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectLightDir), 0.0), specularShininess);
    vec3 specularLight = specularStrength * spec * lightColor;

    // Combined components of lighting
    vec3 phongLight = ambientLight + diffuseLight + specularLight;

    vec3 resultColor = phongLight * objectColor;

    gl_FragColor = vec4(resultColor, 1.0);
}
