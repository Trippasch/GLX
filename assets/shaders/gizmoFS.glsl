#version 330 core
#extension GL_ARB_shading_language_420pack : enable

out vec4 FragColor;

uniform vec3 color;

void main()
{
    FragColor = vec4(color, 1.0);
}