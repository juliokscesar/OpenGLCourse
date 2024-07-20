#version 330 core

#define MAX_NUMBER_SAMPLER2D 15

struct Material
{
    sampler2D texture_diffuse[MAX_NUMBER_SAMPLER2D];
    sampler2D texture_specular[MAX_NUMBER_SAMPLER2D];
    float shininess;
};
uniform Material u_material;
uniform bool u_useMaterial;

in vec2 TexCoords;

void main()
{
    vec4 resultColor = vec4(1.0f);
    
    if (u_useMaterial)
        resultColor = texture(u_material.texture_diffuse[0], TexCoords);

    gl_FragColor = resultColor;
}

