#version 330 core

out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;
in float dist;

uniform sampler2D texture0; // sampler is a data-type for texture obejcts
uniform sampler2D texture1;

void main()
{
    gl_FragColor = mix(texture(texture0, TexCoord), texture(texture1, TexCoord), 0.5);
}
