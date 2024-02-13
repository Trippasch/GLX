#version 330 core
#extension GL_ARB_explicit_uniform_location : enable

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VS_OUT
{
    vec2 TexCoords;
    vec3 WorldPos;
    vec3 Normal;
    vec3 FragPos;
    vec4 FragPosLightSpace;
} vs_out;

layout (std140) uniform Matrices
{
    mat4 projView;
};

layout (location = 1) uniform mat4 model;
layout (location = 2) uniform mat4 lightSpaceMatrix;

void main()
{
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.FragPosLightSpace = (lightSpaceMatrix * vec4(vs_out.FragPos, 1.0));

    vs_out.TexCoords = aTexCoords;
    vs_out.WorldPos = vec3(model * vec4(aPos, 1.0));
    vs_out.Normal = mat3(model) * aNormal;

    gl_Position = projView * vec4(vs_out.WorldPos, 1.0);
}