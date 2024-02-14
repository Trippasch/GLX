#version 330 core
#extension GL_ARB_explicit_uniform_location : enable

layout (location = 0) in vec3 aPos;

out VS_OUT
{
    vec3 FragPos;
} vs_out;

layout (std140) uniform Matrices
{
    mat4 projView;
};
layout (location = 1) uniform mat4 model;

void main()
{
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));

    gl_Position = projView * vec4(vs_out.FragPos, 1.0);
}