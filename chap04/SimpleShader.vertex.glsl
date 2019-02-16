#version 130

in vec4 in_Position;
in vec4 in_Color;
out vec4 ex_Color;

uniform mat4 g_a_model;
uniform mat4 g_a_view;
uniform mat4 g_a_project;

void main(void)
{
    gl_Position = (g_a_project * g_a_view * g_a_model) * in_Position;
    ex_Color = in_Color;
}
