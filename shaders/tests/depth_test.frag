#version 330 core

in vec2 TexCoords;

uniform sampler2D texUnit;

void main()
{
    gl_FragColor = texture(texUnit, TexCoords);
}

