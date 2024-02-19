#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) in vec3 aPos;

layout (std140, binding = 0) uniform Matrices
{
    mat4 projView;
};

layout (location = 1) uniform mat4 model;

void main()
{
    gl_Position = projView * model * vec4(aPos, 1.0);
}