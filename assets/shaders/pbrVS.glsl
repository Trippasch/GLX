#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VS_OUT
{
    vec2 TexCoords;
    vec3 WorldPos;
    vec3 Normal;
} vs_out;

layout (std140, binding = 0) uniform Matrices
{
    mat4 projView;
};
layout (location = 1) uniform mat4 model;

void main()
{
    vs_out.TexCoords = aTexCoords;
    vs_out.WorldPos = vec3(model * vec4(aPos, 1.0));
    vs_out.Normal = mat3(model) * aNormal;

    gl_Position = projView * vec4(vs_out.WorldPos, 1.0);
}