#version 330 core

#define MAX_NUMBER_SAMPLER2D 15

struct Material
{
    sampler2D texture_diffuse[MAX_NUMBER_SAMPLER2D];
    sampler2D texture_specular[MAX_NUMBER_SAMPLER2D];
    float tiling_factor;
    float shininess;
};
uniform Material u_material;
uniform bool u_useMaterial;

in vec2 TexCoords;

void main()
{
    vec4 resultColor = vec4(1.0);
    
    if (u_useMaterial)
    {
        resultColor = texture(u_material.texture_diffuse[0], TexCoords * u_material.tiling_factor);
	if (resultColor.a < 0.5)
	    discard;
    }

    gl_FragColor = resultColor;
}

