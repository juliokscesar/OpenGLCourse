#version 330 core

in vec2 texCoord;
uniform sampler2D texUnit;

void main()
{
    gl_FragColor = texture(texUnit, texCoord);
}
