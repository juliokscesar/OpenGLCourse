#version 330 core

uniform vec3 u_outlineColor;

void main()
{
    gl_FragColor = vec4(u_outlineColor, 1.0);
}
