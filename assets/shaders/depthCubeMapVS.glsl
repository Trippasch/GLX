#version 330 core
#extension GL_ARB_explicit_uniform_location : enable

layout (location = 0) in vec3 aPos;

layout (location = 1) uniform mat4 model;

void main()
{
    gl_Position = model * vec4(aPos, 1.0);
}