#version 460 core
in vec3 v_vertexColors;

uniform mat4 u_ModelMatrix;

out vec4 color;
void main()
{
    color = vec4(v_vertexColors.r, v_vertexColors.g, v_vertexColors.b, 1.0f);
}