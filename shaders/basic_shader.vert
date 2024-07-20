#version 330 core

layout (location = 0) in vec3 a_Pos;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoords;

out vec2 TexCoords;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
    TexCoords = a_TexCoords;

    gl_Position = u_projection * u_view * u_model * vec4(a_Pos, 1.0);
}
