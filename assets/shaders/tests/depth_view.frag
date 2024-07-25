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


float near = 0.1;
float far = 100.0;
float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // back to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}


void main()
{
    vec4 resultColor = vec4(1.0f);
    
    if (u_useMaterial)
        resultColor = texture(u_material.texture_diffuse[0], TexCoords * u_material.tiling_factor);

    // visualizing depth-buffer
    // resultColor = vec4(vec3(gl_FragCoord.z), 1.0);

    float linearDepth = LinearizeDepth(gl_FragCoord.z) / far;
    resultColor = vec4(vec3(linearDepth), 1.0);

    gl_FragColor = resultColor;
}

