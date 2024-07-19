#version 330 core

#define MAX_NUMBER_SAMPLER2D 15

in vec3 FragPos;
in vec3 FragNormal;
in vec2 TexCoords;

struct Material
{
    sampler2D texture_diffuse[MAX_NUMBER_SAMPLER2D];
    sampler2D texture_specular[MAX_NUMBER_SAMPLER2D];
};
uniform Material u_material;

void main()
{
    gl_FragColor = texture(u_material.texture_diffuse[0], TexCoords);
    //gl_FragColor = vec4(FragPos + FragNormal, 1.0);
}

